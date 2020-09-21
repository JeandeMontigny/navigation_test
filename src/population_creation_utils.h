// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef POPULATION_CREATION_UTILS_
#define POPULATION_CREATION_UTILS_

#include "human.h"
#include "behavior.h"
#include "sim-param.h"
#include "geom_construct.h"

namespace bdm {

// ---------------------------------------------------------------------------
  static std::vector<Double3> GetSeatsPositionList() {
    std::vector<Double3> seats_list;
    for (int j = -5; j < 7; j++) {
      double x_position = j*75+45;
      // left rows
      seats_list.push_back({x_position, -95, -40});
      seats_list.push_back({x_position, -50, -40});
      // right rows
      if (j!=2 && j!=3) {
        seats_list.push_back({x_position, 95, -40});
        seats_list.push_back({x_position, 50, -40});
      }
    }

    return seats_list;
  } // end GetSeatsPositionList

// ---------------------------------------------------------------------------
  static std::vector<Double3> GetAgentsPositionList() {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();

    std::vector<Double3> agents_position_list;

    auto get_agents_position_list = [&agents_position_list](const auto* neighbor) {
      auto* hu = bdm_static_cast<const Human*>(neighbor);
      agents_position_list.push_back(hu->GetPosition());
    };
    rm->ApplyOnAllElements(get_agents_position_list);

    return agents_position_list;
  } // end GetAgentsPositionList

// ---------------------------------------------------------------------------
  static bool IsExit(Double3 position) {
    return (position[0] > 190 && position[0] < 210 &&
            position[1] > 100 && position[1] < 120);
  } // end IsExit

// ---------------------------------------------------------------------------
  static bool SamePosition(Double3 seat, Double3 agent) {
    return (agent[0] > seat[0]-20 && agent[0] < seat[0]+20 &&
            agent[1] > seat[1]-20 && agent[1] < seat[1]+20);
  } // end SamePosition

// ---------------------------------------------------------------------------
  static bool SeatTaken(Double3 seat, Double3 searching_agent) {

    // get agent list, except agent at agent_position
    std::vector<Double3> agents_position_list = GetAgentsPositionList();
    agents_position_list.erase(std::remove(agents_position_list.begin(), agents_position_list.end(), searching_agent), agents_position_list.end());

    // check if one agent is at seat position
    for (size_t i = 0; i < agents_position_list.size(); i++) {
      if (SamePosition(seat, agents_position_list[i])) {
        return true;
      }
    }
    return false;
  } // end SeatTaken

// ---------------------------------------------------------------------------
  static Double3 GetEmptySeat() {
    auto* sim = Simulation::GetActive();
    auto* random = sim->GetRandom();

    // check empty seats
    std::vector<Double3> agents_position_list = GetAgentsPositionList();
    std::vector<Double3> seats_list = GetSeatsPositionList();
    std::vector<Double3> empty_seats_list;

    for (size_t i = 0; i < seats_list.size(); i++) {
      bool taken = false;
      for (size_t j = 0; j < agents_position_list.size(); j++) {
        if (SamePosition(seats_list[i], agents_position_list[j])) {
          taken = true;
          break;
        }
      }
      if (!taken) {
        empty_seats_list.push_back(seats_list[i]);
      }
    }

    //TODO: if empty_seats_list is empty,
    //      either stay here, or go to empty stading place

    // get a random empty seat as destination
    Double3 dest = empty_seats_list[(int)random->Uniform(0, empty_seats_list.size())];

    return dest;
  } // end GetEmptySeat()

}

#endif // POPULATION_CREATION_UTILS_
