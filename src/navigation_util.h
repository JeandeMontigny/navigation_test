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
inline double GetBDMToMapLoc(double bdm_loc) {
  auto* sim = Simulation::GetActive();
  auto* param = sim->GetParam();
  auto* sparam = param->GetModuleParam<SimParam>();

  return (bdm_loc + param->max_bound_)/sparam->map_pixel_size;
}
// ---------------------------------------------------------------------------
inline double GetMapToBDMLoc(double map_loc) {
  auto* sim = Simulation::GetActive();
  auto* param = sim->GetParam();
  auto* sparam = param->GetModuleParam<SimParam>();

  return (map_loc * sparam->map_pixel_size) - param->max_bound_;
}

// ---------------------------------------------------------------------------
inline double GetMapSize() {
  auto* sim = Simulation::GetActive();
  auto* param = sim->GetParam();
  auto* sparam = param->GetModuleParam<SimParam>();

  return (param->max_bound_*2)/sparam->map_pixel_size;
}

// ---------------------------------------------------------------------------
  inline std::vector<std::vector<bool>> GetNavigationMap() {
    auto* sim = Simulation::GetActive();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();

    double map_size = GetMapSize();
    std::vector<std::vector<bool>> navigation_map(map_size, std::vector<bool>(map_size, true));

    for (int x = 0 ; x < map_size ; x += sparam->map_pixel_size) {
      for (int y = 0; y < map_size ; y += sparam->map_pixel_size) {
        Double3 position = {x + param->min_bound_, y + param->min_bound_, 0.0};
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
  // destinations_list.push_back(make_pair(GetBDMToMapLoc(450), GetBDMToMapLoc(-450)));

  //TODO: remove hard coded destination
  destinations_list.push_back(make_pair(GetBDMToMapLoc(140), GetBDMToMapLoc(90)));

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
