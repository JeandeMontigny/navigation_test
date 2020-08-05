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

namespace bdm {

// ---------------------------------------------------------------------------
struct Navigation : public BaseBiologyModule {
  BDM_STATELESS_BM_HEADER(Navigation, BaseBiologyModule, 1);

  Navigation() : BaseBiologyModule(gAllEventIds) {}

  Navigation(std::vector<std::vector<bool>> navigation_map) : BaseBiologyModule(gAllEventIds) {
    navigation_map_ = navigation_map;
  }


  void Run(SimObject* so) override {
    // auto* sim = Simulation::GetActive();
    // auto* random = sim->GetRandom();
    // auto* param = sim->GetParam();
    // auto* sparam = param->GetModuleParam<SimParam>();

    // auto* human = bdm_static_cast<Human*>(so);
    //
    // const auto& position = human->GetPosition();

  } // end Run

private:
  std::vector<std::vector<bool>> navigation_map_;
}; // end Navigation

}  // namespace bdm

#endif  // BEHAVIOR_H_
