// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef NAVIGATION_UTIL_
#define NAVIGATION_UTIL_

#include "geom.h"

namespace bdm {

// ---------------------------------------------------------------------------
  inline std::vector<std::vector<bool>> GetNavigationMap() {
    auto* sim = Simulation::GetActive();
    auto* param = sim->GetParam();

    std::vector<std::vector<bool>> navigation_map(param->max_bound_ *2 -1, std::vector<bool>(param->max_bound_*2 -1, true));

    for (int x = 0 ; x < param->max_bound_ *2 -1 ; x++) {
      for (int y = 0; y < param->max_bound_ *2 -1 ; y++) {
        Double3 position = {x +0.5 +param->min_bound_, y +0.5 +param->min_bound_, 0.0};
        if (IsInsideStructure(position)) {
          navigation_map[x][y] = false;
        }
        // if IsInsideStructure() can not capture small geom:
        // use ObjectInbetween(), for x, y and z direction of voxel
        // ie. ObjectInbetween(x, x+1); ObjectInbetween(y, y+1)
      }
    }
    std::cout << "navigation map created" << std::endl;
    return navigation_map;
  } // end GetNavigationMap

// ---------------------------------------------------------------------------
inline std::vector<Pair> AddDestinationToList(std::vector<Pair> destinations_list) {
  //TODO: create destination point depending on the environment:
  //      going to a seat? is destination a wall? etc.
  //      destination also depending on human previously created (same seat?)
  // list of list to check if destination is already taken?
  destinations_list.push_back(make_pair(900, 50));

  return destinations_list;
} // end AddDestinationToList

// ---------------------------------------------------------------------------
inline std::vector<Pair> GetFirstDestination() {
  std::vector<Pair> destinations_list;
  destinations_list = AddDestinationToList(destinations_list);

  return destinations_list;
} // end GetDestinationsList

}

#endif // NAVIGATION_UTIL_
