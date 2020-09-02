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
  static void AddPassenger(int number_of_passenger, State state,
                           std::vector<std::vector<bool>>* navigation_map) {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();
    auto* random = sim->GetRandom();

    //TODO: check empty seats, and select a random one
    // create new human at bus entrance and add this seat as destination

  } // end AddPassenger

// ---------------------------------------------------------------------------
  static void InitialBusPopulationCreation() {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();
    auto* random = sim->GetRandom();

    Human* human;
    // driver: kHealthy, GetInfectedBehaviour
    human = new Human({-475, -65, 0});
    human->SetDiameter(sparam->human_diameter);
    human->state_ = State::kHealthy;
    human->AddBiologyModule(new GetInfectedBehaviour());
    rm->push_back(human);
    // passenger already in the bus
    // kHealthy, GetInfectedBehaviour
    Double3 passenger_position_list [10] = {, };

    for (int i = 0; i < passenger_position_list.size(); i++) {
      human = new Human(passenger_position_list[i]);
      human->SetDiameter(sparam->human_diameter);
      human->state_ = State::kHealthy;
      human->AddBiologyModule(new GetInfectedBehaviour());
      rm->push_back(human);
    }
    // passenger already in bus
    // State::kInfected, SpreadVirusBehaviour
    human = new Human({x, y, z});
    human->SetDiameter(sparam->human_diameter);
    human->state_ = State::kInfected;
    human->AddBiologyModule(new SpreadVirusBehaviour());
    rm->push_back(human);

  } // end BusPopulationCreation

}

#endif // POPULATION_CREATION_
