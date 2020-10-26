// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny and Rachit Belwariar.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef A_STAR_H_
#define A_STAR_H_

#include "navigation_util.h"

namespace bdm {

  // structure to hold the neccesary parameters
  struct node {
      // Row and Column index of its parent
      // Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
      double parent_i, parent_j;
      // f = g + h
      double f, g, h;
  };

// ---------------------------------------------------------------------------
  // check whether given node (row, col) is a valid node or not.
  inline bool IsValid(int row, int col, int sim_size) {
      // Returns true if row number and column number
      // is in range
      return (row >= 0) && (row < sim_size) &&
             (col >= 0) && (col < sim_size);
  }

// ---------------------------------------------------------------------------
  // check whether the given node is blocked or not
  inline bool IsUnBlocked(std::vector<std::vector<bool>> grid, int row, int col) {
    // Returns true if the node is not blocked else false
    if (grid[row][col] == 1) {
      return true;
    }
    return false;
  }

// ---------------------------------------------------------------------------
  // check whether destination node has been reached or not
  inline bool IsDestination(int row, int col, std::pair<double, double> dest) {
    if (row == dest.first && col == dest.second) {
      return true;
    }
    return false;
  }

// ---------------------------------------------------------------------------
  // calculate the 'h' heuristics.
  inline double CalculateHValue(int row, int col, std::pair<double, double> dest) {
    // Return using the distance formula
    return ((double)sqrt ((row-dest.first)*(row-dest.first)
                          + (col-dest.second)*(col-dest.second)));
  }

// ---------------------------------------------------------------------------
  // trace the path from the destination to the source
  inline std::vector<std::vector<double>> TracePath(std::vector<std::vector<node>> nodeDetails, std::pair<double, double> dest) {

    double row = dest.first;
    double col = dest.second;
    std::vector<std::vector<double>> path;

    while (!(nodeDetails[row][col].parent_i == row
             && nodeDetails[row][col].parent_j == col )) {
      path.push_back({row, col});
      double temp_row = nodeDetails[row][col].parent_i;
      double temp_col = nodeDetails[row][col].parent_j;
      row = temp_row;
      col = temp_col;
    }
    path.push_back({row, col});

    return path;
  } // end TracePath

// ---------------------------------------------------------------------------
  // find the shortest path between a given source node to a destination
  // node according to A* Search Algorithm
  inline std::vector<std::vector<double>> AStar(std::vector<std::vector<bool>> grid,
                           std::pair<double, double> src, std::pair<double, double> dest, const int sim_size) {

    std::vector<std::vector<double>> path;

    // If the source is out of range
    if (IsValid (src.first, src.second, sim_size) == false) {
      std::cout << "source " << GetMapToBDMLoc(src.first) << ", "
           << GetMapToBDMLoc(src.second)
           << " is out of simulation space" << std::endl;
      return path;
    }

    // If the destination is out of range
    if (IsValid (dest.first, dest.second, sim_size) == false) {
      std::cout << "destination " << GetMapToBDMLoc(dest.first) << ", "
           << GetMapToBDMLoc(dest.second)
           << " is out of simulation space" << std::endl;
      return path;
    }

    // Either the source or the destination is blocked
    if (IsUnBlocked(grid, src.first, src.second) == false ||
        IsUnBlocked(grid, dest.first, dest.second) == false) {
          std::cout << "source " << GetMapToBDMLoc(src.first) << ", "
               << GetMapToBDMLoc(src.second) << " or destination "
               << GetMapToBDMLoc(dest.first) << ", "
               << GetMapToBDMLoc(dest.second)
               << " are blocked (position not allowed)" << std::endl;
      return path;
    }

    // If the destination node is the same as source node
    if (IsDestination(src.first, src.second, dest) == true) {
      std::cout << "source "  << GetMapToBDMLoc(src.first) << ", "
           << GetMapToBDMLoc(src.second)
           << " is the destination" << std::endl;
      return path;
    }

    // Create a 2D vector of bool closed list and initialise it to false
    // which means that no node has been included yet
    std::vector<std::vector<bool>> closedList(sim_size, std::vector<bool>(sim_size, false));

    // Declare a 2D vector to hold the details of that node
    std::vector<std::vector<node>> nodeDetails(sim_size, std::vector<node>(sim_size));

    int i, j;

    for (i = 0; i < sim_size; i++) {
      for (j = 0; j < sim_size; j++) {
        nodeDetails[i][j].f = FLT_MAX;
        nodeDetails[i][j].g = FLT_MAX;
        nodeDetails[i][j].h = FLT_MAX;
        nodeDetails[i][j].parent_i = -1;
        nodeDetails[i][j].parent_j = -1;
      }
    }

    // Initialising the parameters of the starting node
    i = src.first, j = src.second;
    nodeDetails[i][j].f = 0.0;
    nodeDetails[i][j].g = 0.0;
    nodeDetails[i][j].h = 0.0;
    nodeDetails[i][j].parent_i = i;
    nodeDetails[i][j].parent_j = j;

    // Create an open list having information as-
    // <f, <i, j>>
    // where f = g + h,
    // and i, j are the row and column index of that node
    // Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
    // This open list is implenented as a set of std::pair of std::pair.
    std::set<std::pair<double, std::pair<double, double>>> openList;

    // Put the starting node on the open list and set its
    // 'f' as 0
    openList.insert(std::make_pair (0.0, std::make_pair (i, j)));

    while (!openList.empty()) {
      std::pair<double, std::pair<double, double>> p = *openList.begin();

      // Remove this vertex from the open list
      openList.erase(openList.begin());

      // Add this vertex to the closed list
      i = p.second.first;
      j = p.second.second;
      closedList[i][j] = true;

      // To store the 'g', 'h' and 'f' of the 4 successors
      double gNew, hNew, fNew;

      //----------- 1st Successor (North) ------------
      // Only process this node if this is a valid one
      if (IsValid(i-1, j, sim_size) == true) {
        // If the destination node is the same as the
        // current successor
        if (IsDestination(i-1, j, dest) == true) {
          // Set the Parent of the destination node
          nodeDetails[i-1][j].parent_i = i;
          nodeDetails[i-1][j].parent_j = j;
          path = TracePath(nodeDetails, dest);
          return path;
        }
        // If the successor is already on the closed
        // list or if it is blocked, then ignore it.
        // Else do the following
        else if (closedList[i-1][j] == false &&
                 IsUnBlocked(grid, i-1, j) == true) {
          gNew = nodeDetails[i][j].g + 1.0;
          hNew = CalculateHValue (i-1, j, dest);
          fNew = gNew + hNew;

          // If it isn’t on the open list, add it to
          // the open list. Make the current square
          // the parent of this square. Record the
          // f, g, and h costs of the square node
          //                OR
          // If it is on the open list already, check
          // to see if this path to that square is better,
          // using 'f' cost as the measure.
          if (nodeDetails[i-1][j].f == FLT_MAX ||
              nodeDetails[i-1][j].f > fNew) {
            openList.insert( std::make_pair(fNew, std::make_pair(i-1, j)));

            // Update the details of this node
            nodeDetails[i-1][j].f = fNew;
            nodeDetails[i-1][j].g = gNew;
            nodeDetails[i-1][j].h = hNew;
            nodeDetails[i-1][j].parent_i = i;
            nodeDetails[i-1][j].parent_j = j;
          }
        }
      }

      //----------- 2nd Successor (South) ------------
      // Only process this node if this is a valid one
      if (IsValid(i+1, j, sim_size) == true) {
        // If the destination node is the same as the
        // current successor
        if (IsDestination(i+1, j, dest) == true) {
          // Set the Parent of the destination node
          nodeDetails[i+1][j].parent_i = i;
          nodeDetails[i+1][j].parent_j = j;
          path = TracePath(nodeDetails, dest);
          return path;
        }

        else if (closedList[i+1][j] == false &&
                 IsUnBlocked(grid, i+1, j) == true) {
          gNew = nodeDetails[i][j].g + 1.0;
          hNew = CalculateHValue(i+1, j, dest);
          fNew = gNew + hNew;

          if (nodeDetails[i+1][j].f == FLT_MAX ||
              nodeDetails[i+1][j].f > fNew) {
            openList.insert( std::make_pair (fNew, std::make_pair (i+1, j)));
            // Update the details of this node
            nodeDetails[i+1][j].f = fNew;
            nodeDetails[i+1][j].g = gNew;
            nodeDetails[i+1][j].h = hNew;
            nodeDetails[i+1][j].parent_i = i;
            nodeDetails[i+1][j].parent_j = j;
          }
        }
      }

      //----------- 3rd Successor (East) ------------
      // Only process this node if this is a valid one
      if (IsValid (i, j+1, sim_size) == true) {
        // If the destination node is the same as the
        // current successor
        if (IsDestination(i, j+1, dest) == true) {
          // Set the Parent of the destination node
          nodeDetails[i][j+1].parent_i = i;
          nodeDetails[i][j+1].parent_j = j;
          path = TracePath(nodeDetails, dest);
          return path;
        }

        else if (closedList[i][j+1] == false &&
                 IsUnBlocked (grid, i, j+1) == true) {
          gNew = nodeDetails[i][j].g + 1.0;
          hNew = CalculateHValue (i, j+1, dest);
          fNew = gNew + hNew;

          if (nodeDetails[i][j+1].f == FLT_MAX ||
              nodeDetails[i][j+1].f > fNew) {
            openList.insert( std::make_pair(fNew, std::make_pair (i, j+1)));

            // Update the details of this node
            nodeDetails[i][j+1].f = fNew;
            nodeDetails[i][j+1].g = gNew;
            nodeDetails[i][j+1].h = hNew;
            nodeDetails[i][j+1].parent_i = i;
            nodeDetails[i][j+1].parent_j = j;
          }
        }
      }

      //----------- 4th Successor (West) ------------
      // Only process this node if this is a valid one
      if (IsValid(i, j-1, sim_size) == true) {
        // If the destination node is the same as the
        // current successor
        if (IsDestination(i, j-1, dest) == true) {
          // Set the Parent of the destination node
          nodeDetails[i][j-1].parent_i = i;
          nodeDetails[i][j-1].parent_j = j;
          path = TracePath(nodeDetails, dest);
          return path;
        }

        else if (closedList[i][j-1] == false &&
                 IsUnBlocked(grid, i, j-1) == true) {
          gNew = nodeDetails[i][j].g + 1.0;
          hNew = CalculateHValue(i, j-1, dest);
          fNew = gNew + hNew;

          if (nodeDetails[i][j-1].f == FLT_MAX ||
              nodeDetails[i][j-1].f > fNew) {
            openList.insert( std::make_pair (fNew, std::make_pair (i, j-1)));

            // Update the details of this node
            nodeDetails[i][j-1].f = fNew;
            nodeDetails[i][j-1].g = gNew;
            nodeDetails[i][j-1].h = hNew;
            nodeDetails[i][j-1].parent_i = i;
            nodeDetails[i][j-1].parent_j = j;
          }
        }
      }

      } // end !openList.empty
      // When the open list is empty, the destination node has not been found
      std::cout << "warning: couldn't find a way from "
                << GetMapToBDMLoc(src.first) << ", " << GetMapToBDMLoc(src.second)
                << " (map loc: " << src.first << ", " << src.second << ") to "
                << GetMapToBDMLoc(dest.first) << ", " << GetMapToBDMLoc(dest.second)
                << " (map loc: " << dest.first << ", " << dest.second << ")"
                << std::endl;
      return path;
  } // end aStarSearch

} // namespace bdm


#endif // A_STAR_H_
