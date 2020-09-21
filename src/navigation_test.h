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

  // grid too big if not reduced
  int grid_spacing = sparam->map_pixel_size;
  int resolution = param->max_bound_*2/grid_spacing;
  // ratio diffusion_coef/spacing/spacing = 0.125
  double diffusion_coef = 0.166*grid_spacing*grid_spacing;
  double decay_const = 0;

  //construct geom
  BuildBus();
  // construct the 2d array for navigation
  std::vector<std::vector<bool>> navigation_map;

  // print navigation_map
  // for (int map_x = 0; map_x < navigation_map.size(); map_x++) {
  //   for (int map_y = 0; map_y < navigation_map[0].size(); map_y++) {
  //     std::cout << navigation_map[map_x][map_y];
  //   }
  //   std::cout << std::endl;
  // }

  // define substance for virus concentration
  ModelInitializer::DefineSubstance(dg_0_, "aerosol", diffusion_coef,
                                    decay_const, resolution);
  ModelInitializer::DefineSubstance(dg_1_, "droplets", diffusion_coef,
                                    decay_const, resolution);
  std::cout << "substance initialised" << std::endl;

  // humans creation
  // HumanCreator(-2400, 2400, -1400, 1400, 20, State::kHealthy, &navigation_map);
  // HumanCreator(-2400, 2400, -1400, 1400, 1, State::kInfected, &navigation_map);

  //bus population creation
  // InitialBusPopulationCreation(&navigation_map);

  auto* rm = simulation.GetResourceManager();
  Human* human = new Human({0, 0, 0});
  human->SetDiameter(sparam->human_diameter);
  human->state_ = State::kInfected;
  human->AddBiologyModule(new SpreadVirusBehaviour());
  rm->push_back(human);

  std::cout << "population created" << std::endl;

  // Run simulation for number_of_steps timestep
  std::cout << "simulating.." << std::endl;
  for (uint64_t i = 0; i <= sparam->number_of_steps; ++i) {
    simulation.GetScheduler()->Simulate(1);
    if (i > 0 && i % 1000 == 0) {
      std::ofstream diffusion_export;
      std::string export_fileName = Concat(param->output_dir_, "/step", i, ".txt").c_str();
      diffusion_export.open(export_fileName);

      DiffusionGrid* dg = nullptr;
      dg = rm->GetDiffusionGrid("virus");

      for (int x_pos = 0; x_pos < param->max_bound_*2; x_pos++) {

        diffusion_export << x_pos-param->max_bound_ << " "
        << dg->GetConcentration({(double)x_pos-param->max_bound_, 0, 0}) << "\n";
      }
      diffusion_export.close();

      std::cout << "x 212: " << dg->GetConcentration({-212.0, 0.0, 0.0})
                << " ; max/x 212 ratio: "
                << dg->GetConcentration({-212.5, 0.0, 0.0})/dg->GetConcentration({-12.5, 0.0, 0.0})
                <<  std::endl;

      std::cout << "x 712: " << dg->GetConcentration({-712.0, 0.0, 0.0})
                << " ; max/x 712 ratio: "
                << dg->GetConcentration({-712.5, 0.0, 0.0})/dg->GetConcentration({-12.5, 0.0, 0.0})
                <<  std::endl;

      std::cout << "step " << i << " out of "
                << sparam->number_of_steps << std::endl;

    }
    // TODO passenger pop in at bus entrace position
    // add destination to an empty seat
    // first bus stop
    // if (i == 50) {
    //   AddPassenger(State::kHealthy, &navigation_map);
    // }
    // if (i == 100) {
    //   AddPassenger(State::kHealthy, &navigation_map);
    // }
    // if (i == 150) {
    //   AddPassenger(State::kHealthy, &navigation_map);
    // }
    // if (i == 200) {
    //   AddPassenger(State::kHealthy, &navigation_map);
    // }
    // if (i == 250) {
    //   AddPassenger(State::kHealthy, &navigation_map);
    // }
    // if (i == 300) {
    //   AddPassenger(State::kHealthy, &navigation_map);
    // }
    // if (i == 350) {
    //   AddPassenger(State::kHealthy, &navigation_map);
    // }
    // // second bus stop
    // if (i == 2050) {
    //   AddPassenger(State::kInfected, &navigation_map);
    // }
    // if (i == 2100) {
    //   AddPassenger(State::kHealthy, &navigation_map);
    // }
    // if (i == 2150) {
    //   AddPassenger(State::kHealthy, &navigation_map);
    // }
  }

  std::cout << "done" << std::endl;
  return 0;
}

}  // namespace bdm

#endif  // NAVIGATION_TEST_H_
