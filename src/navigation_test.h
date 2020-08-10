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
#include "human.h"
#include "behavior.h"
#include "sim-param.h"
#include "geom.h"
#include "util_methods.h"
#include "navigation_util.h"
#include "a_star.h"

namespace bdm {

inline int Simulate(int argc, const char** argv) {
  bdm::Param::RegisterModuleParam(new bdm::SimParam());

  Simulation simulation(argc, argv);

  auto* param = simulation.GetParam();
  auto* sparam = param->GetModuleParam<SimParam>();
  auto* rm = simulation.GetResourceManager();
  simulation.GetRandom()->SetSeed(2975); // rand() % 10000

  //construct geom
  BuildMaze();
  // construct the 2d array for navigation
  std::vector<std::vector<bool>> navigation_map = GetNavigationMap();

  // human creation
  Human* human = new Human({-450, 450, 0});
  human->SetDiameter(sparam->human_diameter);
  // human->AddBiologyModule(new Navigation());
  human->AddBiologyModule(new Navigation(&navigation_map));
  rm->push_back(human);

  Node start;
  start.x = 50; start.y = 900;
  Node dest;
  dest.x = 900; dest.y = 50;

  // for (int x = 0; x < navigation_map.size(); x++ ) {
  //   for (int y = 0; y < navigation_map[0].size(); y++ ) {
  //     std::cout << navigation_map[x][y];
  //   }
  //   std::cout << std::endl;
  // }

  std::cout << "call A*" << std::endl;
  AStar(navigation_map, start, dest);

  // Run simulation for number_of_steps timestep
  for (uint64_t i = 0; i < sparam->number_of_steps; ++i) {
    simulation.GetScheduler()->Simulate(1);
  }

  std::cout << "done" << std::endl;
  return 0;
}

}  // namespace bdm

#endif  // NAVIGATION_TEST_H_
