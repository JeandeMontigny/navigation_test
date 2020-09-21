// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef BEHAVIOR_H_
#define BEHAVIOR_H_

#include "core/biology_module/biology_module.h"
#include "human.h"
#include "geom_construct.h"
#include "sim-param.h"
#include "a_star.h"
#include "navigation_util.h"
#include "core/scheduler.h"
#include "core/diffusion_grid.h"
#include "core/resource_manager.h"
#include "population_creation_utils.h"

namespace bdm {

  // enumerate substances in simulation
  enum Substances { dg_0_ , dg_1_};

// ---------------------------------------------------------------------------
struct Navigation : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(Navigation, BaseBiologyModule, 1);

  Navigation() : BaseBiologyModule(gAllEventIds) {}

  Navigation(std::vector<std::vector<bool>>* navigation_map) : BaseBiologyModule(gAllEventIds) {
    navigation_map_ = navigation_map;
  }


  void Run(SimObject* so) override {
    auto* sim = Simulation::GetActive();
    auto* random = sim->GetRandom();

    // execute this BM only evey x steps to allow diffusion
    if (sim->GetScheduler()->GetSimulatedSteps() % 5 != 0) {
      return;
    }

    auto* human = bdm_static_cast<Human*>(so);
    const auto& position = human->GetPosition();
    std::vector<double> previous_position = {position[0], position[1]};

    // if human is at the bus exit
    if (IsExit(position)) {
      human->RemoveFromSimulation();
    }

    // some passengers (x%) leave if sim step == y
    // add destination to bus exit
    if (sim->GetScheduler()->GetSimulatedSteps() % 2000 == 0
        && random->Uniform() < 0.2) {
      human->destinations_list_ =
        AddDestinationToList(human->destinations_list_, navigation_map_,
                             200, 200, 110, 110);
    }

    std::vector<std::vector<double>> path;

    // if agent has to calculate path to destination
    if (!path_calculated_ && !human->destinations_list_.empty()) {
      std::pair<double, double> start =
        std::make_pair(GetBDMToMapLoc(position[0]),
                       GetBDMToMapLoc(position[1]));
      std::pair<double, double> dest = human->destinations_list_[0];

      // calculate path using A*
      path = AStar((*navigation_map_), start, dest, navigation_map_->size());

      human->path_ = path;
      // remove this travel form destination_list
      human->destinations_list_.erase(human->destinations_list_.begin());
      path_calculated_ = true;
    } // end if has to calculate path

    // if agent has its path, has to move to it
    else if (path_calculated_) {
      Double3 dest = {GetMapToBDMLoc(human->path_[0][0]),
                      GetMapToBDMLoc(human->path_[0][1]), -40};
      // if human is close to destination, check if seat is still empty
      if (human->path_.size() < 25 && !IsExit(dest) &&
        SeatTaken(dest, position)) {
        dest = GetEmptySeat();
        std::vector<std::pair<double, double>> destinations_list;
        destinations_list.push_back(std::make_pair(
          GetBDMToMapLoc(dest[0]), GetBDMToMapLoc(dest[1])));
        human->destinations_list_= destinations_list;
        path_calculated_ = false;
        return;
      }

      if (!human->path_.empty()) {
        Double3 next_position = {
          GetMapToBDMLoc(human->path_[human->path_.size()-1][0]),
          GetMapToBDMLoc(human->path_[human->path_.size()-1][1]),
          position[2] };
        // navigate according to path
        human->SetPosition(next_position);

        double vec_x = human->GetPosition()[0] - previous_position[0];
        double vec_y = human->GetPosition()[1] - previous_position[1];
        double vec_norm = std::sqrt((vec_x*vec_x)+(vec_y*vec_y));
        human->orientation_ = {vec_x/vec_norm, vec_y/vec_norm};

        // if either on or after this path position
        human->path_.erase(human->path_.end());
      }
      // path is empty, so destination is reached
      else {
        // agent orientation follow bus orientation
        human->orientation_ = {-1, 0};
        // can add an other destination here
        path_calculated_ = false;
      }
    } // end has its path

  } // end Run

private:
  bool path_calculated_ = false;
  std::vector<std::vector<bool>>* navigation_map_;
}; // end Navigation

// ---------------------------------------------------------------------------
struct SpreadVirusBehaviour : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(SpreadVirusBehaviour, BaseBiologyModule, 1);

  SpreadVirusBehaviour() : BaseBiologyModule(gAllEventIds) {}

  void Run(SimObject* so) override {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* random = sim->GetRandom();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();

    auto* human = bdm_static_cast<Human*>(so);
    Double3 position = human->GetPosition();
    double radius = human->GetDiameter()/2;

    human->orientation_ = {0, 1};

    // recovery time if infected
    if (human->state_ == State::kInfected) {
    //   if (human->recovery_counter_ <= 0) {
    //     human->state_ = State::kRecovered;
    //   } else {
    //     human->recovery_counter_--;
    //   }

    // virus spreading
    DiffusionGrid* dg_a = nullptr;
    dg_a = rm->GetDiffusionGrid("aerosol");
    DiffusionGrid* dg_d = nullptr;
    dg_d = rm->GetDiffusionGrid("droplets");

    int grid_spacing = sparam->map_pixel_size;
    std::vector<Double3> diffusion_positions;

    double d_len = 100;
    double d_wid = 100;
    double v[2] = {human->orientation_[0], human->orientation_[1]};

    // breathing spread
    // no significant production of droplets
    // low produciton of aerosol and short propagation
    if (random->Uniform() < 1) { // 0.95
      // coordinates of triangle where substance is produced
      // mouth
      Double3 tri_a =
        { position[0] + v[0] * radius,
          position[1] + v[1] * radius, position[2]};
      // further point in agent's orientation, at d_len dist
      Double3 tri_d =
        { tri_a[0] + v[0] * d_len,
          tri_a[1] + v[1] * d_len, position[2]};
      // triangle base coordinates
      // B = 90° of v fom D * d_wid/2
      double theta = DegToRad(90);
      double cs = std::cos(theta);
      double sn = std::sin(theta);
      double w_b[2] = {v[0] * cs - v[1] * sn, v[0] * sn + v[1] * cs};
      Double3 tri_b =
        { tri_d[0] + w_b[0] * d_wid/2,
          tri_d[1] + w_b[1] * d_wid/2,
          position[2]};
      // C = -90° of v fom D * d_wid/2
      theta = DegToRad(-90);
      cs = std::cos(theta);
      sn = std::sin(theta);
      double w_c[2] = {v[0] * cs - v[1] * sn, v[0] * sn + v[1] * cs};
      Double3 tri_c =
      { tri_d[0] + w_c[0] * d_wid/2,
        tri_d[1] + w_c[1] * d_wid/2,
        position[2]};
      std::cout << "tri_a: " << tri_a[0] << ", " << tri_a[1]
                << "; tri_d: " << tri_d[0] << ", " << tri_d[1]
                << "; tri_b: " << tri_b[0] << ", " << tri_b[1]
                << "; tri_c: " << tri_c[0] << ", " << tri_c[1]
                << std::endl;
      // for each diffusion point in from of agent (up to 1m)
      for (int i = 0; i < 100/grid_spacing; i++) {
        // for diffusion points within cone width (1m)
        for (int j = 0; j < 100/grid_spacing; j++) {
          // potential diffusion point
          double diff_x = tri_a[0]
            + v[0] * i * grid_spacing;
          double diff_y = tri_a[1]
            + v[1] * j * grid_spacing;
          double diff_z = position[2];
          Double3 point_pos = {diff_x, diff_y, diff_z};
          //if i,j is within cone
          if (IsInsideTriangle(tri_a, tri_b, tri_c, point_pos)) {
            diffusion_positions.push_back(point_pos);
          }
        } // end for jj
      } // end for i
    } // end if normal breathing

    // cough or sneez spread
    // long distance and high concentration. rarely occurs
    else {
      // cough
      if (random->Uniform() < 0.5) {
        return;
      }
      // cough
      else {
        return;
      }
    }

    std::cout << diffusion_positions.size()
              << " points to diffuse to"
              // << "; first one: "
              // << diffusion_positions[0][0] << " " << diffusion_positions[0][1]
              << "; last one: "
              << diffusion_positions[diffusion_positions.size()-1][0] << " " << diffusion_positions[diffusion_positions.size()-1][1]
              << std::endl;

    for (int point = 0; point < diffusion_positions.size(); point++) {
      // TODO: decrease diffusion when far from emission point
      dg_a->IncreaseConcentrationBy(diffusion_positions[point], 8);
    }

    // dg_d->IncreaseConcentrationBy(diffusion_position, 8);
    } // end if kInfected

  } // end Run
}; // end SpreadVirusBehaviour

// ---------------------------------------------------------------------------
struct GetInfectedBehaviour : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(GetInfectedBehaviour, BaseBiologyModule, 1);

  GetInfectedBehaviour() : BaseBiologyModule(gAllEventIds) {}

  void Run(SimObject* so) override {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();

    auto* human = bdm_static_cast<Human*>(so);

    // nothing to do if already infected, so remove BM
    if (human->state_ == State::kInfected) {
      human->RemoveBiologyModule(this);
      return;
    }

    // incubation time if incubation
    // if (human->state_ == State::kIncubation) {
    //   if (human->recovery_counter_ <= 0) {
    //     human->state_ = State::kInfected;
    //     human->AddBiologyModule(new SpreadVirusBehaviour());
    //   } else {
    //     human->incubation_counter_--;
    //   }
    // } // end if kIncubation

    // infection
    if (human->state_ == State::kHealthy) {
      DiffusionGrid* dg_a = nullptr;
      DiffusionGrid* dg_d = nullptr;
      dg_a = rm->GetDiffusionGrid("aerosol");
      dg_d = rm->GetDiffusionGrid("droplets");
      double aerosol = dg_a->GetConcentration(human->GetPosition());
      double droplets = dg_d->GetConcentration(human->GetPosition());
      double concentration = aerosol + droplets;

      if (concentration > 1e-6) {
        human->state_ = State::kIncubation;
      }
    } // end infection

  } // end Run
}; // end GetInfectedBehaviour

}  // namespace bdm

#endif  // BEHAVIOR_H_
