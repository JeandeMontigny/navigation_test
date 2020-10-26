// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef HUMAN_H_
#define HUMAN_H_

#include "core/sim_object/cell.h"
#include "core/biology_module/biology_module.h"

namespace bdm {

enum State { kHealthy, kIncubation, kInfected, kRecovered };

class Human : public Cell {
  BDM_SIM_OBJECT_HEADER(Human, Cell, 1);

 public:
  Human() {}
  Human(const Event& event, SimObject* other, uint64_t new_oid = 0)
      : Base(event, other, new_oid) {}
  explicit Human(const Double3& position) : Base(position) {}

  // This data member stores the current state of the agent.
  int state_ = State::kHealthy;
  int incubation_counter_ = 1e9;
  int recovery_counter_ = 1e9;
  // store the destinations
  std::vector<std::pair<double, double>> destinations_list_;
  // store the path to a destination
  std::vector<std::vector<double>> path_;
  /// store the agent's orientation. by default, follow bus orientation
  std::vector<double> orientation_ = {-1,0};
};

}  // namespace bdm

#endif // HUMAN_H_
