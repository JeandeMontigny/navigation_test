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
#include "openlb_export.h"
#include "openlb_sim.h"

namespace bdm {

inline int Simulate(int argc, const char** argv) {
  bdm::Param::RegisterModuleParam(new bdm::SimParam());

  Simulation simulation(argc, argv);

  auto* param = simulation.GetParam();
  auto* sparam = param->GetModuleParam<SimParam>();
  simulation.GetRandom()->SetSeed(2975); // rand() % 10000
  // choose if print navigation map
  bool print_navigation_map = false;
  // create OpenLB directories and files
  std::string openlbDir = Concat(param->output_dir_, "/../../openlb/");

  //construct geom
  BuildBus();

  // construct the 2d array for navigation
  std::vector<std::vector<bool>> navigation_map = GetNavigationMap(-40);

  // print navigation_map
  if (print_navigation_map) {
    for (size_t map_x = 0; map_x < navigation_map.size(); map_x++) {
      for (size_t map_y = 0; map_y < navigation_map[0].size(); map_y++) {
        std::cout << navigation_map[map_x][map_y];
      }
      std::cout << std::endl;
    }
  }

  //bus population creation
  InitialBusPopulationCreation(&navigation_map);
  std::cout << "population created" << std::endl;

  ExportOpenlbFiles(openlbDir);

  int o_argc; char* o_argv[1];
  openlb_sim::main(o_argc, o_argv);

  return 1;

  // Run simulation for number_of_steps timestep
  std::cout << "simulating.." << std::endl;
  for (uint64_t i = 0; i <= sparam->number_of_steps; ++i) {
    simulation.GetScheduler()->Simulate(1);

    // passenger pop in at bus entrace position
    // first bus stop
    // TODO: Navigation is run every time step. check arrival timing
    if (i == 10) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 20) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 30) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 40) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 50) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 60) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 70) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    // second bus stop
    if (i == 810) {
      AddPassenger(State::kInfected, &navigation_map);
    }
    if (i == 820) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 830) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    // third bus stop
    if (i == 1810) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 1820) {
      AddPassenger(State::kInfected, &navigation_map);
    }
    if (i == 1830) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 1840) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
    if (i == 1850) {
      AddPassenger(State::kHealthy, &navigation_map);
    }
  }

  std::cout << "done" << std::endl;
  return 0;
}

}  // namespace bdm

#endif  // NAVIGATION_TEST_H_
