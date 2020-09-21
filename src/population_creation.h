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
#include "geom_construct.h"
#include "population_creation_utils.h"

namespace bdm {

  static void AddPassenger(State state,
                           std::vector<std::vector<bool>>* navigation_map) {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();

    // agent created at bus entrance
    Human* human = new Human({-480, 110, -40});
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

    // search for an empty seat
    Double3 dest = GetEmptySeat();
    std::vector<std::pair<double, double>> destinations_list;
    destinations_list.push_back(std::make_pair(
      GetBDMToMapLoc(dest[0]), GetBDMToMapLoc(dest[1])));
    human->destinations_list_= destinations_list;

    rm->push_back(human);
  } // end AddPassenger

// ---------------------------------------------------------------------------
  static void InitialBusPopulationCreation(std::vector<std::vector<bool>>* navigation_map) {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();

    Human* human;
    // driver: kHealthy, GetInfectedBehaviour
    human = new Human({-475, -65, -40});
    human->SetDiameter(sparam->human_diameter);
    human->state_ = State::kHealthy;
    human->AddBiologyModule(new GetInfectedBehaviour());
    rm->push_back(human);
    // passenger already in the bus
    // kHealthy, Navigation, GetInfectedBehaviour
    std::vector<Double3> passenger_position_list = {
      {-105, -50, -40}, {-30, -50, -40}, {420, -95, -40}, {-30, -95, -40},
      {45, 95, -40}, {120, 95, -40}, {-180, 50, -40}, {-30, 50, -40},
      {-30, 95, -40}, {495, -95, -40}
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
    human = new Human({45, -95, -40});
    human->SetDiameter(sparam->human_diameter);
    human->state_ = State::kInfected;
    human->AddBiologyModule(new Navigation(navigation_map));
    human->AddBiologyModule(new SpreadVirusBehaviour());
    rm->push_back(human);

  } // end BusPopulationCreation

}

#endif // POPULATION_CREATION_
