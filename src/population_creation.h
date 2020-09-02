// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef POPULATION_CREATION_
#define POPULATION_CREATION_

#include "human.h"
#include "behavior.h"
#include "sim-param.h"
#include "geom.h"

namespace bdm {

  // define human creator
  static void HumanCreator(double min_x, double max_x,
                           double min_y, double max_y,
                           int num_human, State state,
                           std::vector<std::vector<bool>>* navigation_map) {

    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();
    auto* random = sim->GetRandom();

    if (min_x <= param->min_bound_ || min_y <= param->min_bound_ ||
        max_x >= param->max_bound_ || max_y >= param->max_bound_) {
      std::cout << "human creation outside of simulation space!" << std::endl;
      return;
    }

    double x, y;
    double z = 150;

    for (int i = 0; i < num_human; i++) {
      do {
        x = random->Uniform(min_x, max_x);
        y = random->Uniform(min_y, max_y);
      } while (IsInsideStructure({x, y, z}));

      Human* human = new Human({x, y, z});
      human->SetDiameter(sparam->human_diameter);
      human->state_ = state;

      // get destinations for this human
      std::vector<std::pair<double, double>> destinations_list = GetFirstDestination(navigation_map, min_x, max_x, min_y, max_y);
      destinations_list = AddDestinationToList(destinations_list, navigation_map,
                                               min_x, max_x, min_y, max_y);
      destinations_list = AddDestinationToList(destinations_list, navigation_map,
                                               1500, 1500, -1400, -1400);

      human->destinations_list_= destinations_list;
      // add biology modules
      if (state == State::kInfected) {
        human->AddBiologyModule(new SpreadVirusBehaviour());
      }
      human->AddBiologyModule(new Navigation(navigation_map));
      human->AddBiologyModule(new GetInfectedBehaviour());
      rm->push_back(human);
    }
  }  // end CellCreator

  // ---------------------------------------------------------------------------
    static std::vector<Double3> GetSeatsList() {
      std::vector<Double3> seats_list;
      for (int j = -5; j < 7; j++) {
        double x_position = j*75+45;
        // left rows
        seats_list.push_back({x_position, -95, 0});
        seats_list.push_back({x_position, -50, 0});
        // right rows
        if (j!=2 && j!=3) {
          seats_list.push_back({x_position, 95, 0});
          seats_list.push_back({x_position, 50, 0});
        }
      }

      return seats_list;
    } // end GetSeatsList

// ---------------------------------------------------------------------------
  static bool SeatTaken(Double3 seat, Double3 agent) {
    return ((agent[0] > seat[0]-20 || agent[0] < seat[0]+20) &&
            (agent[1] > seat[1]-20 || agent[1] < seat[1]+20));
  } // end SeatTaken

// ---------------------------------------------------------------------------
  static void AddPassenger(int number_of_passenger, State state,
                           std::vector<std::vector<bool>>* navigation_map) {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();
    auto* random = sim->GetRandom();

    // agent created at bus entrance + waiting queu in y direction
    // TODO: queu in x direction, alongside the bus?
    Human* human;
    for (int i = 0; i < number_of_passenger; i ++) {
      human = new Human({-500, (double)130 + 55 * i, 0});
      human->SetDiameter(sparam->human_diameter);
      human->state_ = state;
      // add biology modules
      human->AddBiologyModule(new Navigation(navigation_map));
      if (state == State::kHealthy) {
        human->AddBiologyModule(new GetInfectedBehaviour());
      }
      else {
        human->AddBiologyModule(new SpreadVirusBehaviour());
      }
      // check empty seats
      std::vector<Double3> agents_position_list;
      std::vector<Double3> seats_list = GetSeatsList();
      std::vector<Double3> empty_seats_list;

      auto get_empty_seats = [&agents_position_list](const auto* neighbor) {
        auto* hu = bdm_static_cast<const Human*>(neighbor);
        agents_position_list.push_back(hu->GetPosition());
      };

      rm->ApplyOnAllElements(get_empty_seats);

      for (size_t i = 0; i < seats_list.size(); i++) {
        bool taken = false;
        for (size_t j = 0; j < agents_position_list.size(); j++) {
          if (SeatTaken(seats_list[i], agents_position_list[j])) {
            taken = true;
            break;
          }
        }
        if (!taken) {
          empty_seats_list.push_back(seats_list[i]);
        }
      }

      // add a random empty seat as destination
      auto dest = empty_seats_list[(int)random->Uniform(0, empty_seats_list.size())];
      std::vector<std::pair<double, double>> destinations_list;
      destinations_list.push_back(std::make_pair(dest[0], dest[1]));

      human->destinations_list_= destinations_list;

      rm->push_back(human);
    } // end for number_of_passenger
  } // end AddPassenger

// ---------------------------------------------------------------------------
  static void InitialBusPopulationCreation(std::vector<std::vector<bool>>* navigation_map) {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();

    Human* human;
    // driver: kHealthy, GetInfectedBehaviour
    human = new Human({-475, -65, 0});
    human->SetDiameter(sparam->human_diameter);
    human->state_ = State::kHealthy;
    human->AddBiologyModule(new GetInfectedBehaviour());
    rm->push_back(human);
    // passenger already in the bus
    // kHealthy, Navigation, GetInfectedBehaviour
    std::vector<Double3> passenger_position_list = {
      {-50, -50, 0}, {-30, -50, 0}, {420, -95, 0}, {-30, -95, 0}, {45, 95, 0},
      {120, 95, 0}, {-180, 50, 0}, {-30, 50, 0}, {-30, 95, 0}, {495, -95, 0}
    };

    for (size_t i = 0; i < passenger_position_list.size(); i++) {
      human = new Human(passenger_position_list[i]);
      human->SetDiameter(sparam->human_diameter);
      human->state_ = State::kHealthy;
      human->AddBiologyModule(new Navigation(navigation_map));
      human->AddBiologyModule(new GetInfectedBehaviour());
      rm->push_back(human);
    }
    // passenger already in bus
    // State::kInfected, SpreadVirusBehaviour
    human = new Human({45, -95, 0});
    human->SetDiameter(sparam->human_diameter);
    human->state_ = State::kInfected;
    human->AddBiologyModule(new Navigation(navigation_map));
    human->AddBiologyModule(new SpreadVirusBehaviour());
    rm->push_back(human);

  } // end BusPopulationCreation

}

#endif // POPULATION_CREATION_
