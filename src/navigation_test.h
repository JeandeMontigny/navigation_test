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
  // get destinations for this human
  std::vector<Pair> destinations_list = GetFirstDestination();
  human->destinations_list_= destinations_list;
  human->AddBiologyModule(new Navigation(&navigation_map));
  rm->push_back(human);

  // Run simulation for number_of_steps timestep
  for (uint64_t i = 0; i < sparam->number_of_steps; ++i) {
    simulation.GetScheduler()->Simulate(1000);
  }

  std::cout << "done" << std::endl;
  return 0;
}

}  // namespace bdm

#endif  // NAVIGATION_TEST_H_
