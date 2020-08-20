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
#include "geom.h"
#include "sim-param.h"
#include "a_star.h"
#include "navigation_util.h"

namespace bdm {

  // enumerate substances in simulation
  enum Substances { dg_0_ };

// ---------------------------------------------------------------------------
struct Navigation : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(Navigation, BaseBiologyModule, 1);

  Navigation() : BaseBiologyModule(gAllEventIds) {}

  Navigation(std::vector<std::vector<bool>>* navigation_map) : BaseBiologyModule(gAllEventIds) {
    navigation_map_ = navigation_map;
  }


  void Run(SimObject* so) override {
    auto* sim = Simulation::GetActive();
    // execute this BM only evey x steps to allow diffusion
    if (sim->GetScheduler()->GetSimulatedSteps() % 5 != 0) {
      return;
    }

    auto* human = bdm_static_cast<Human*>(so);
    const auto& position = human->GetPosition();

    // if human is at the supermarket exit
    if ((position[0] > 1480 || position[0] < 1520) &&
        (position[1] > -1520 || position[1] < -1480)) {
      human->RemoveFromSimulation();
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

      if (!human->path_.empty()) {
        Double3 next_position = {
          GetMapToBDMLoc(human->path_[human->path_.size()-1][0]),
          GetMapToBDMLoc(human->path_[human->path_.size()-1][1]),
          position[2] };
        // navigate according to path
        human->SetPosition(next_position);

        // if either on or after this path position
        human->path_.erase(human->path_.end());
      }
      // path is empty, so destination is reached
      else {
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

    auto* human = bdm_static_cast<Human*>(so);

    // recovery time if infected
    if (human->state_ == State::kInfected) {
      if (human->recovery_counter_ <= 0) {
        human->state_ = State::kRecovered;
      } else {
        human->recovery_counter_--;
      }

    // virus spreading
    DiffusionGrid* dg = nullptr;
    dg = rm->GetDiffusionGrid("virus");
    dg->IncreaseConcentrationBy(human->GetPosition(), 1);
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

    // incubation time if incubation
    if (human->state_ == State::kIncubation) {
      if (human->recovery_counter_ <= 0) {
        human->state_ = State::kInfected;
        human->AddBiologyModule(new SpreadVirusBehaviour());
      } else {
        human->incubation_counter_--;
      }
    } // end if kIncubation

    // infection
    if (human->state_ == State::kHealthy) {
      DiffusionGrid* dg = nullptr;
      dg = rm->GetDiffusionGrid("virus");
      double concentration = dg->GetConcentration(human->GetPosition());

      if (concentration > 0.001) {
        human->state_ = State::kIncubation;
      }
    } // end infection

  } // end Run
}; // end GetInfectedBehaviour

}  // namespace bdm

#endif  // BEHAVIOR_H_
