// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef A_STAR_H_
#define A_STAR_H_

#include <bits/stdc++.h>

namespace bdm {

// ---------------------------------------------------------------------------
  struct Node {
    int x;
    int y;
    int parentX;
    int parentY;
    float gCost;
    float hCost;
    float fCost;
  };

// ---------------------------------------------------------------------------
  inline bool operator < (const Node& lhs, const Node& rhs) {
    return lhs.fCost < rhs.fCost;
  }

// ---------------------------------------------------------------------------
  // check if node is an obstacle or outside map
  inline bool IsValid(std::vector<std::vector<bool>>* map,
                      int x, int y, int max_bound) {
    if (x < 0 || x > max_bound || y < 0 || y > max_bound || (*map)[x][y]) {
      return false;
    }
    else {
      return true;
    }
  }

// ---------------------------------------------------------------------------
  // check if node x, y is the destination
  inline bool IsDestination(int x, int y, Node dest) {
    if (x == dest.x && y == dest.y) {
      return true;
    }
    return false;
  }

// ---------------------------------------------------------------------------
  inline double CalculateH(int x, int y, Node dest) {
      return (double)(sqrt((x - dest.x)*(x - dest.x) + (y - dest.y)*(y - dest.y)));
  }

// ---------------------------------------------------------------------------
  inline std::vector<Node> GetPath(std::array<std::array<Node, 999>, 999> map, Node dest) {
    std::cout << "Found a path" << std::endl;
    int x = dest.x;
    int y = dest.y;
    std::stack<Node> path;
    std::vector<Node> usablePath;

    while (!(map[x][y].parentX == x && map[x][y].parentY == y)
           && map[x][y].x != -1 && map[x][y].y != -1) {
      path.push(map[x][y]);
      int tempX = map[x][y].parentX;
      int tempY = map[x][y].parentY;
      x = tempX;
      y = tempY;
    }
    path.push(map[x][y]);

    while (!path.empty()) {
      Node top = path.top();
      path.pop();
      usablePath.emplace_back(top);
    }

    return usablePath;
  } // end MakePath

// ---------------------------------------------------------------------------
  inline std::vector<Node> AStar(std::vector<std::vector<bool>> map, Node start, Node dest) {
    std::cout << "entered A*" << std::endl;
    int max_bound = map.size();
    std::vector<Node> empty;

    if (IsValid(&map, dest.x, dest.y, max_bound) == false) {
      std::cout << "destination is not valid (obstacle or outside)" << std::endl;
      return empty;
    }

    if (IsDestination(start.x, start.y, dest)) {
      std::cout << "starting point is the destination" << std::endl;
      return empty;
    }

    bool closedList[max_bound][max_bound];
    // initialize map
    // std::array<std::array < Node, max_bound>, max_bound> allMap;
    std::array<std::array < Node, 999>, 999> allMap;

    for (int x = 0; x < max_bound; x++) {
      for (int y = 0; y < max_bound; y++) {
        allMap[x][y].fCost = FLT_MAX;
        allMap[x][y].gCost = FLT_MAX;
        allMap[x][y].hCost = FLT_MAX;
        allMap[x][y].parentX = -1;
        allMap[x][y].parentY = -1;
        allMap[x][y].x = x;
        allMap[x][y].y = y;

        closedList[x][y] = false;
      }
    }

    //Initialize our starting list
    int x = start.x;
    int y = start.y;
    allMap[x][y].fCost = 0.0;
    allMap[x][y].gCost = 0.0;
    allMap[x][y].hCost = 0.0;
    allMap[x][y].parentX = x;
    allMap[x][y].parentY = y;

    std::vector<Node> openList;
    openList.emplace_back(allMap[x][y]);
    bool destinationFound = false;

    while (!openList.empty()&&openList.size()<max_bound*max_bound) {
      Node node;
      do {
        float temp = FLT_MAX;
        std::vector<Node>::iterator itNode;
        for (std::vector<Node>::iterator it = openList.begin();
        it != openList.end(); it = next(it)) {
          Node n = *it;
          if (n.fCost < temp) {
            temp = n.fCost;
            itNode = it;
          }
        }
        node = *itNode;
        openList.erase(itNode);
      } while (IsValid(&map, node.x, node.y, max_bound) == false);

      x = node.x;
      y = node.y;
      closedList[x][y] = true;

      //For each neighbour starting from North-West to South-East
      for (int newX = -1; newX <= 1; newX++) {
        for (int newY = -1; newY <= 1; newY++) {
          double gNew, hNew, fNew;
          if (IsValid(&map, x + newX, y + newY, max_bound)) {
            if (IsDestination(x + newX, y + newY, dest)) {
              //Destination found - make path
              allMap[x + newX][y + newY].parentX = x;
              allMap[x + newX][y + newY].parentY = y;
              destinationFound = true;
              return GetPath(allMap, dest);
            }
            else if (closedList[x + newX][y + newY] == false) {
              gNew = node.gCost + 1.0;
              hNew = CalculateH(x + newX, y + newY, dest);
              fNew = gNew + hNew;
              // Check if this path is better than the one already present
              if (allMap[x + newX][y + newY].fCost == FLT_MAX ||
                  allMap[x + newX][y + newY].fCost > fNew) {
                // Update the details of this neighbour node
                allMap[x + newX][y + newY].fCost = fNew;
                allMap[x + newX][y + newY].gCost = gNew;
                allMap[x + newX][y + newY].hCost = hNew;
                allMap[x + newX][y + newY].parentX = x;
                allMap[x + newX][y + newY].parentY = y;
                openList.emplace_back(allMap[x + newX][y + newY]);
              }
            }
          }
        }
      }
    } // while not empty
    if (destinationFound == false) {
      std::cout << "Destination not found" << std::endl;
      return empty;
    }
  } // end AStar


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
  // #include <bits/stdc++.h>
//   using namespace std;
//
//   #define ROW 999
//   #define COL 999
//
//   // Creating a shortcut for int, int pair type
//   typedef pair<int, int> Pair;
//
//   // Creating a shortcut for pair<int, pair<int, int>> type
//   typedef pair<double, pair<int, int>> pPair;
//
//   // A structure to hold the neccesary parameters
//   struct mapCell {
//       // Row and Column index of its parent
//       // Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
//       int parent_i, parent_j;
//       // f = g + h
//       double f, g, h;
//   };
//
//   // A Utility Function to check whether given mapCell (row, col)
//   // is a valid mapCell or not.
//   inline bool IsValid(int row, int col) {
//       // Returns true if row number and column number
//       // is in range
//       return (row >= 0) && (row < ROW) &&
//              (col >= 0) && (col < COL);
//   }
//
//   // A Utility Function to check whether the given mapCell is
//   // blocked or not
//   inline bool isUnBlocked(std::vector<std::vector<bool>> grid, int row, int col) {
//       // Returns true if the mapCell is not blocked else false
//       if (grid[row][col] == 1)
//           return (true);
//       else
//           return (false);
//   }
//
//   // A Utility Function to check whether destination mapCell has
//   // been reached or not
//   inline bool IsDestination(int row, int col, Pair dest) {
//       if (row == dest.first && col == dest.second)
//           return (true);
//       else
//           return (false);
//   }
//
//   // A Utility Function to calculate the 'h' heuristics.
//   inline double CalculateHValue(int row, int col, Pair dest) {
//       // Return using the distance formula
//       return ((double)sqrt ((row-dest.first)*(row-dest.first)
//                             + (col-dest.second)*(col-dest.second)));
//   }
//
//   // A Utility Function to trace the path from the source
//   // to destination
//   inline void tracePath(mapCell mapCellDetails[][COL], Pair dest) {
//       printf ("\nThe Path is ");
//       int row = dest.first;
//       int col = dest.second;
//
//       stack<Pair> Path;
//
//       while (!(mapCellDetails[row][col].parent_i == row
//                && mapCellDetails[row][col].parent_j == col )) {
//           Path.push (make_pair (row, col));
//           int temp_row = mapCellDetails[row][col].parent_i;
//           int temp_col = mapCellDetails[row][col].parent_j;
//           row = temp_row;
//           col = temp_col;
//       }
//
//       Path.push (make_pair (row, col));
//       while (!Path.empty()) {
//           pair<int,int> p = Path.top();
//           Path.pop();
//           printf("-> (%d,%d) ",p.first,p.second);
//       }
//
//       return;
//   }
//
//   // A Function to find the shortest path between
//   // a given source mapCell to a destination mapCell according
//   // to A* Search Algorithm
//   inline void aStarSearch(std::vector<std::vector<bool>> grid, Pair src, Pair dest) {
//       // If the source is out of range
//       if (IsValid (src.first, src.second) == false) {
//           printf ("Source is invalid\n");
//           return;
//       }
//
//       // If the destination is out of range
//       if (IsValid (dest.first, dest.second) == false) {
//           printf ("Destination is invalid\n");
//           return;
//       }
//
//       // Either the source or the destination is blocked
//       if (isUnBlocked(grid, src.first, src.second) == false ||
//               isUnBlocked(grid, dest.first, dest.second) == false) {
//           printf ("Source or the destination is blocked\n");
//           return;
//       }
//
//       // If the destination mapCell is the same as source mapCell
//       if (IsDestination(src.first, src.second, dest) == true) {
//           printf ("We are already at the destination\n");
//           return;
//       }
//
//       // Create a closed list and initialise it to false which means
//       // that no mapCell has been included yet
//       // This closed list is implemented as a boolean 2D array
//       bool closedList[ROW][COL];
//       memset(closedList, false, sizeof (closedList));
//
//       // Declare a 2D array of structure to hold the details
//       //of that mapCell
//       mapCell mapCellDetails[ROW][COL];
//
//       int i, j;
//
//       for (i=0; i<ROW; i++) {
//           for (j=0; j<COL; j++) {
//               mapCellDetails[i][j].f = FLT_MAX;
//               mapCellDetails[i][j].g = FLT_MAX;
//               mapCellDetails[i][j].h = FLT_MAX;
//               mapCellDetails[i][j].parent_i = -1;
//               mapCellDetails[i][j].parent_j = -1;
//           }
//       }
//
//       // Initialising the parameters of the starting node
//       i = src.first, j = src.second;
//       mapCellDetails[i][j].f = 0.0;
//       mapCellDetails[i][j].g = 0.0;
//       mapCellDetails[i][j].h = 0.0;
//       mapCellDetails[i][j].parent_i = i;
//       mapCellDetails[i][j].parent_j = j;
//
//       /*
//        Create an open list having information as-
//        <f, <i, j>>
//        where f = g + h,
//        and i, j are the row and column index of that mapCell
//        Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
//        This open list is implenented as a set of pair of pair.*/
//       set<pPair> openList;
//
//       // Put the starting mapCell on the open list and set its
//       // 'f' as 0
//       openList.insert(make_pair (0.0, make_pair (i, j)));
//
//       // We set this boolean value as false as initially
//       // the destination is not reached.
//       bool foundDest = false;
//
//       while (!openList.empty()) {
//           pPair p = *openList.begin();
//
//           // Remove this vertex from the open list
//           openList.erase(openList.begin());
//
//           // Add this vertex to the closed list
//           i = p.second.first;
//           j = p.second.second;
//           closedList[i][j] = true;
//
//           /*
//           Generating all the 4 successor of this mapCell
//
//                     N
//                     |
//                     |
//               W----Cell----E
//                     |
//                     |
//                     S
//
//           Cell-->Popped Cell (i, j)
//           N -->  North       (i-1, j)
//           S -->  South       (i+1, j)
//           E -->  East        (i, j+1)
//           W -->  West           (i, j-1)
//           */
//
//           // To store the 'g', 'h' and 'f' of the 8 successors
//           double gNew, hNew, fNew;
//
//           //----------- 1st Successor (North) ------------
//
//           // Only process this mapCell if this is a valid one
//           if (IsValid(i-1, j) == true) {
//               // If the destination mapCell is the same as the
//               // current successor
//               if (IsDestination(i-1, j, dest) == true) {
//                   // Set the Parent of the destination mapCell
//                   mapCellDetails[i-1][j].parent_i = i;
//                   mapCellDetails[i-1][j].parent_j = j;
//                   printf ("The destination mapCell is found\n");
//                   tracePath (mapCellDetails, dest);
//                   foundDest = true;
//                   return;
//               }
//               // If the successor is already on the closed
//               // list or if it is blocked, then ignore it.
//               // Else do the following
//               else if (closedList[i-1][j] == false &&
//                        isUnBlocked(grid, i-1, j) == true) {
//                   gNew = mapCellDetails[i][j].g + 1.0;
//                   hNew = CalculateHValue (i-1, j, dest);
//                   fNew = gNew + hNew;
//
//                   // If it isn’t on the open list, add it to
//                   // the open list. Make the current square
//                   // the parent of this square. Record the
//                   // f, g, and h costs of the square mapCell
//                   //                OR
//                   // If it is on the open list already, check
//                   // to see if this path to that square is better,
//                   // using 'f' cost as the measure.
//                   if (mapCellDetails[i-1][j].f == FLT_MAX ||
//                           mapCellDetails[i-1][j].f > fNew) {
//                       openList.insert( make_pair(fNew,
//                                                  make_pair(i-1, j)));
//
//                       // Update the details of this mapCell
//                       mapCellDetails[i-1][j].f = fNew;
//                       mapCellDetails[i-1][j].g = gNew;
//                       mapCellDetails[i-1][j].h = hNew;
//                       mapCellDetails[i-1][j].parent_i = i;
//                       mapCellDetails[i-1][j].parent_j = j;
//                   }
//               }
//           }
//
//           //----------- 2nd Successor (South) ------------
//
//           // Only process this mapCell if this is a valid one
//           if (IsValid(i+1, j) == true) {
//               // If the destination mapCell is the same as the
//               // current successor
//               if (IsDestination(i+1, j, dest) == true) {
//                   // Set the Parent of the destination mapCell
//                   mapCellDetails[i+1][j].parent_i = i;
//                   mapCellDetails[i+1][j].parent_j = j;
//                   printf("The destination mapCell is found\n");
//                   tracePath(mapCellDetails, dest);
//                   foundDest = true;
//                   return;
//               }
//               // If the successor is already on the closed
//               // list or if it is blocked, then ignore it.
//               // Else do the following
//               else if (closedList[i+1][j] == false &&
//                        isUnBlocked(grid, i+1, j) == true) {
//                   gNew = mapCellDetails[i][j].g + 1.0;
//                   hNew = CalculateHValue(i+1, j, dest);
//                   fNew = gNew + hNew;
//
//                   // If it isn’t on the open list, add it to
//                   // the open list. Make the current square
//                   // the parent of this square. Record the
//                   // f, g, and h costs of the square mapCell
//                   //                OR
//                   // If it is on the open list already, check
//                   // to see if this path to that square is better,
//                   // using 'f' cost as the measure.
//                   if (mapCellDetails[i+1][j].f == FLT_MAX ||
//                           mapCellDetails[i+1][j].f > fNew) {
//                       openList.insert( make_pair (fNew, make_pair (i+1, j)));
//                       // Update the details of this mapCell
//                       mapCellDetails[i+1][j].f = fNew;
//                       mapCellDetails[i+1][j].g = gNew;
//                       mapCellDetails[i+1][j].h = hNew;
//                       mapCellDetails[i+1][j].parent_i = i;
//                       mapCellDetails[i+1][j].parent_j = j;
//                   }
//               }
//           }
//
//           //----------- 3rd Successor (East) ------------
//
//           // Only process this mapCell if this is a valid one
//           if (IsValid (i, j+1) == true) {
//               // If the destination mapCell is the same as the
//               // current successor
//               if (IsDestination(i, j+1, dest) == true) {
//                   // Set the Parent of the destination mapCell
//                   mapCellDetails[i][j+1].parent_i = i;
//                   mapCellDetails[i][j+1].parent_j = j;
//                   printf("The destination mapCell is found\n");
//                   tracePath(mapCellDetails, dest);
//                   foundDest = true;
//                   return;
//               }
//
//               // If the successor is already on the closed
//               // list or if it is blocked, then ignore it.
//               // Else do the following
//               else if (closedList[i][j+1] == false &&
//                        isUnBlocked (grid, i, j+1) == true) {
//                   gNew = mapCellDetails[i][j].g + 1.0;
//                   hNew = CalculateHValue (i, j+1, dest);
//                   fNew = gNew + hNew;
//
//                   // If it isn’t on the open list, add it to
//                   // the open list. Make the current square
//                   // the parent of this square. Record the
//                   // f, g, and h costs of the square mapCell
//                   //                OR
//                   // If it is on the open list already, check
//                   // to see if this path to that square is better,
//                   // using 'f' cost as the measure.
//                   if (mapCellDetails[i][j+1].f == FLT_MAX ||
//                           mapCellDetails[i][j+1].f > fNew) {
//                       openList.insert( make_pair(fNew,
//                                           make_pair (i, j+1)));
//
//                       // Update the details of this mapCell
//                       mapCellDetails[i][j+1].f = fNew;
//                       mapCellDetails[i][j+1].g = gNew;
//                       mapCellDetails[i][j+1].h = hNew;
//                       mapCellDetails[i][j+1].parent_i = i;
//                       mapCellDetails[i][j+1].parent_j = j;
//                   }
//               }
//           }
//
//           //----------- 4th Successor (West) ------------
//
//           // Only process this mapCell if this is a valid one
//           if (IsValid(i, j-1) == true) {
//               // If the destination mapCell is the same as the
//               // current successor
//               if (IsDestination(i, j-1, dest) == true) {
//                   // Set the Parent of the destination mapCell
//                   mapCellDetails[i][j-1].parent_i = i;
//                   mapCellDetails[i][j-1].parent_j = j;
//                   printf("The destination mapCell is found\n");
//                   tracePath(mapCellDetails, dest);
//                   foundDest = true;
//                   return;
//               }
//
//               // If the successor is already on the closed
//               // list or if it is blocked, then ignore it.
//               // Else do the following
//               else if (closedList[i][j-1] == false &&
//                        isUnBlocked(grid, i, j-1) == true) {
//                   gNew = mapCellDetails[i][j].g + 1.0;
//                   hNew = CalculateHValue(i, j-1, dest);
//                   fNew = gNew + hNew;
//
//                   // If it isn’t on the open list, add it to
//                   // the open list. Make the current square
//                   // the parent of this square. Record the
//                   // f, g, and h costs of the square mapCell
//                   //                OR
//                   // If it is on the open list already, check
//                   // to see if this path to that square is better,
//                   // using 'f' cost as the measure.
//                   if (mapCellDetails[i][j-1].f == FLT_MAX ||
//                           mapCellDetails[i][j-1].f > fNew) {
//                       openList.insert( make_pair (fNew,
//                                             make_pair (i, j-1)));
//
//                       // Update the details of this mapCell
//                       mapCellDetails[i][j-1].f = fNew;
//                       mapCellDetails[i][j-1].g = gNew;
//                       mapCellDetails[i][j-1].h = hNew;
//                       mapCellDetails[i][j-1].parent_i = i;
//                       mapCellDetails[i][j-1].parent_j = j;
//                   }
//               }
//           }
//
//       } // end !openList.empty
//
//       // When the destination mapCell is not found and the open
//       // list is empty, then we conclude that we failed to
//       // reach the destiantion mapCell. This may happen when the
//       // there is no way to destination mapCell (due to blockages)
//       if (foundDest == false)
//           printf("Failed to find the Destination cell\n");
//
//       // return path;
//   } // end aStarSearch
//
} // namespace bdm


#endif // A_STAR_H_
