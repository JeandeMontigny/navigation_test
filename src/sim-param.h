// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef SIM_PARAM_H_
#define SIM_PARAM_H_

#include "core/param/module_param.h"

namespace bdm {

/// This class defines parameters that are specific to this simulation.
struct SimParam : public ModuleParam {
  static const ModuleParamUid kUid;

  ModuleParam* GetCopy() const override;

  ModuleParamUid GetUid() const override;

  SimParam() {}

  uint64_t number_of_steps = 30;
  double human_diameter = 50; // cm
  // std::vector<std::vector<bool>> navigation_map;

 protected:
  /// Assign values from config file to variables
  void AssignFromConfig(const std::shared_ptr<cpptoml::table>&) override;

 private:
  BDM_CLASS_DEF_OVERRIDE(SimParam, 1);
};

}  // namespace bdm

#endif  // SIM_PARAM_H_
