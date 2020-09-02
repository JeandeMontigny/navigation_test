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
  std::vector<std::vector<bool>> navigation_map = GetNavigationMap();

  // define substance for virus concentration
  ModelInitializer::DefineSubstance(dg_0_, "virus", diffusion_coef,
                                    decay_const, resolution);
  //TODO: inactive diffusion grid points for points in structure
  std::cout << "substance initialised" << std::endl;

  // humans creation
  // HumanCreator(-2400, 2400, -1400, 1400, 20, State::kHealthy, &navigation_map);
  // HumanCreator(-2400, 2400, -1400, 1400, 1, State::kInfected, &navigation_map);

  //bus population creation
  InitialBusPopulationCreation(navigation_map);

  // auto* rm = simulation.GetResourceManager();
  // Human* human;
  // // Driver: kHealthy, GetInfectedBehaviour
  // human = new Human({-475, -65, 0});
  // human->SetDiameter(sparam->human_diameter);
  // human->state_ = State::kHealthy;
  // human->AddBiologyModule(new GetInfectedBehaviour());
  // rm->push_back(human);
  //
  // // passenger: kHealthy, GetInfectedBehaviour
  // human = new Human({-95, -100, 0});
  // human->SetDiameter(sparam->human_diameter);
  // human->state_ = State::kHealthy;
  // human->AddBiologyModule(new GetInfectedBehaviour());
  // rm->push_back(human);

  std::cout << "population created" << std::endl;

  // Run simulation for number_of_steps timestep
  std::cout << "simulating.." << std::endl;
  for (uint64_t i = 0; i < sparam->number_of_steps; ++i) {
    simulation.GetScheduler()->Simulate(1);
    // TODO passenger pop in at bus entrace position
    // add destination to an empty seat
    if (i == 100) {
      AddPassenger(1, State::kHealthy, navigation_map);
    }
  }

  std::cout << "done" << std::endl;
  return 0;
}

}  // namespace bdm

#endif  // NAVIGATION_TEST_H_
