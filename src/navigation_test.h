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
#include "geom.h"
// #include "util_methods.h"
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

  // grid too big if not reduced
  int grid_spacing = sparam->map_pixel_size;
  int resolution = param->max_bound_*2/grid_spacing;
  // ratio diffusion_coef/spacing/spacing = 0.125
  double diffusion_coef = 0.125*grid_spacing*grid_spacing;
  double decay_const = 0;

  //construct geom
  BuildBus();
  // construct the 2d array for navigation
  std::vector<std::vector<bool>> navigation_map = GetNavigationMap(-45);

  // print navigation_map
  // for (int map_x = 0; map_x < navigation_map.size(); map_x++) {
  //   for (int map_y = 0; map_y < navigation_map[0].size(); map_y++) {
  //     std::cout << navigation_map[map_x][map_y];
  //   }
  //   std::cout << std::endl;
  // }

  // define substance for virus concentration
  ModelInitializer::DefineSubstance(dg_0_, "virus", diffusion_coef,
                                    decay_const, resolution);
  std::cout << "substance initialised" << std::endl;

  // humans creation
  // HumanCreator(-2400, 2400, -1400, 1400, 20, State::kHealthy, &navigation_map);
  // HumanCreator(-2400, 2400, -1400, 1400, 1, State::kInfected, &navigation_map);

  //bus population creation
  InitialBusPopulationCreation(&navigation_map);

  std::cout << "population created" << std::endl;

  // Run simulation for number_of_steps timestep
  std::cout << "simulating.." << std::endl;
  for (uint64_t i = 0; i <= sparam->number_of_steps; ++i) {
    simulation.GetScheduler()->Simulate(1);
    if (i % 1000 == 0) {
      std::cout << "step " << i << " out of "
                << sparam->number_of_steps << std::endl;
    }
    // TODO passenger pop in at bus entrace position
    // add destination to an empty seat
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
