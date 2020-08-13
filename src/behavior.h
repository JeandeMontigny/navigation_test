// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef BEHAVIOR_H_
#define BEHAVIOR_H_

#include "core/biology_module/biology_module.h"
#include "human.h"
#include "geom.h"
#include "sim-param.h"
#include "a_star.h"

namespace bdm {

// ---------------------------------------------------------------------------
struct Navigation : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(Navigation, BaseBiologyModule, 1);

  Navigation() : BaseBiologyModule(gAllEventIds) {}

  Navigation(std::vector<std::vector<bool>>* navigation_map) : BaseBiologyModule(gAllEventIds) {
    navigation_map_ = navigation_map;
  }


  void Run(SimObject* so) override {
    auto* sim = Simulation::GetActive();
    // auto* random = sim->GetRandom();
    // auto* param = sim->GetParam();
    // auto* sparam = param->GetModuleParam<SimParam>();

    auto* human = bdm_static_cast<Human*>(so);

    // const auto& position = human->GetPosition();

    // std::cout << navigation_map_->size() << std::endl;
    // std::cout << (*navigation_map_)[0][0] << std::endl;

    // if has to calculate path to travel
    if (!path_calculated_) {
      Pair start = make_pair(50, 900);
      Pair dest = make_pair(900, 50);

      auto path = AStar((*navigation_map_), start, dest, navigation_map_->size());

      while (!path.empty()) {
        std::cout << path[path.size()-1][0] << ","
                  << path[path.size()-1][1] << " -> ";
        path.erase(path.end());
      }
      std::cout << "destination" << std::endl;

      path_calculated_ = true;
    } // end if has to calculate path

  } // end Run

private:
  bool path_calculated_ = false;
  std::vector<std::vector<bool>>* navigation_map_;
}; // end Navigation

}  // namespace bdm

#endif  // BEHAVIOR_H_
