// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef NAVIGATION_UTIL_
#define NAVIGATION_UTIL_

#include "geom.h"
#include "sim-param.h"

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

    for (int x = 0 ; x < map_size ; x ++) {
      for (int y = 0; y < map_size ; y ++) {
        double pos_x = GetMapToBDMLoc(x);
        double pos_y = GetMapToBDMLoc(y);
        Double3 position = {pos_x, pos_y, 0.0};
        if (IsInsideStructure(position) ||
            // x axis
            ObjectInbetween({pos_x - sparam->human_diameter/2, pos_y, 0.0},
                            {pos_x + sparam->human_diameter/2, pos_y, 0.0}) ||
            // y axis
            ObjectInbetween({pos_x, pos_y - sparam->human_diameter/2, 0.0},
                            {pos_x, pos_y + sparam->human_diameter/2, 0.0}) ||
            // diagonals
            ObjectInbetween({pos_x - sparam->human_diameter/2 * 0.7,
                             pos_y - sparam->human_diameter/2 * 0.7, 0.0},
                            {pos_x + sparam->human_diameter/2 * 0.7,
                             pos_y + sparam->human_diameter/2 * 0.7, 0.0}) ||
           ObjectInbetween({pos_x - sparam->human_diameter/2 * 0.7,
                            pos_y + sparam->human_diameter/2 * 0.7, 0.0},
                           {pos_x + sparam->human_diameter/2 * 0.7,
                            pos_y - sparam->human_diameter/2 * 0.7, 0.0}) ||
            // z axis
            ObjectInbetween({pos_x, pos_y,-sparam->human_diameter/2},
                            {pos_x, pos_y, sparam->human_diameter/2}) ) {
          navigation_map[x][y] = false;
        }
      }
    }
    std::cout << "navigation map created" << std::endl;
    return navigation_map;
  } // end GetNavigationMap

// ---------------------------------------------------------------------------
inline std::vector<std::pair<double, double>>
 AddDestinationToList(std::vector<std::pair<double, double>> destinations_list,
                      double min_x, double max_x, double min_y, double max_y) {
  //TODO: create destination point depending on the environment:
  //      going to a seat? is destination a wall? etc.
  //      destination also depending on human previously created (same seat?)
  // list of list to check if destination is already taken?

  auto* sim = Simulation::GetActive();
  auto* random = sim->GetRandom();

  double x = random->Uniform(min_x, max_x);
  double y = random->Uniform(min_y, max_y);

  destinations_list.push_back(std::make_pair(GetBDMToMapLoc(x), GetBDMToMapLoc(y)));

  return destinations_list;
} // end AddDestinationToList

// ---------------------------------------------------------------------------
inline std::vector<std::pair<double, double>>
 GetFirstDestination(double min_x, double max_x, double min_y, double max_y) {
  std::vector<std::pair<double, double>> destinations_list;
  destinations_list = AddDestinationToList(destinations_list,
                                           min_x, max_x, min_y, max_y);

  return destinations_list;
} // end GetDestinationsList

} // namespace bdm

#endif // NAVIGATION_UTIL_
