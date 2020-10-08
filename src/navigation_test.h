// -----------------------------------------------------------------------------
//
// Copyright (C) The BioDynaMo Project.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------
#ifndef NAVIGATION_TEST_H_
#define NAVIGATION_TEST_H_

#include "biodynamo.h"
#include "core/simulation.h"
#include "human.h"
#include "behavior.h"
#include "sim-param.h"
#include "geom_construct.h"
#include "geom.h"
#include "util_methods.h"
#include "navigation_util.h"
#include "a_star.h"
#include "population_creation.h"
#include "core/diffusion_grid.h"

namespace bdm {

inline int Simulate(int argc, const char** argv) {
  bdm::Param::RegisterModuleParam(new bdm::SimParam());

  Simulation simulation(argc, argv);

  auto* param = simulation.GetParam();
  auto* sparam = param->GetModuleParam<SimParam>();
  simulation.GetRandom()->SetSeed(2975); // rand() % 10000

  //construct geom
  BuildBus();
  // export ROOT geometry to OpenFOAM format
  ExportGeomToFoam();

  // construct the 2d array for navigation
  std::vector<std::vector<bool>> navigation_map = GetNavigationMap(-40);

  // print navigation_map
  // for (int map_x = 0; map_x < navigation_map.size(); map_x++) {
  //   for (int map_y = 0; map_y < navigation_map[0].size(); map_y++) {
  //     std::cout << navigation_map[map_x][map_y];
  //   }
  //   std::cout << std::endl;
  // }

  //bus population creation
  InitialBusPopulationCreation(&navigation_map);
  std::cout << "population created" << std::endl;

  // Run simulation for number_of_steps timestep
  std::cout << "simulating.." << std::endl;
  for (uint64_t i = 0; i <= sparam->number_of_steps; ++i) {
    simulation.GetScheduler()->Simulate(1);

    // passenger pop in at bus entrace position
    // first bus stop
    if (i == 50) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 100) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 150) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 200) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 250) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 300) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 350) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    // second bus stop
    if (i == 2050) {
      AddPassenger(State::kInfected, &navigation_map);
    }
    if (i == 2100) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 2150) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
  }

  std::cout << "done" << std::endl;
  return 0;
}

}  // namespace bdm

#endif  // NAVIGATION_TEST_H_
