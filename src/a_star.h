// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny and Rachit Belwariar.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef A_STAR_H_
#define A_STAR_H_


namespace bdm {
  using namespace std;

  // Creating a shortcut for int, int pair type
  typedef pair<int, int> Pair;

  // Creating a shortcut for pair<int, pair<int, int>> type
  typedef pair<double, pair<int, int>> pPair;

  // A structure to hold the neccesary parameters
  struct mapCell {
      // Row and Column index of its parent
      // Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
      int parent_i, parent_j;
      // f = g + h
      double f, g, h;
  };

  // A Utility Function to check whether given mapCell (row, col)
  // is a valid mapCell or not.
  bool isValid(int row, int col) {
      // Returns true if row number and column number
      // is in range
      return (row >= 0) && (row < ROW) &&
             (col >= 0) && (col < COL);
  }

  // A Utility Function to check whether the given mapCell is
  // blocked or not
  bool isUnBlocked(int grid[][COL], int row, int col) {
      // Returns true if the mapCell is not blocked else false
      if (grid[row][col] == 1)
          return (true);
      else
          return (false);
  }

  // A Utility Function to check whether destination mapCell has
  // been reached or not
  bool isDestination(int row, int col, Pair dest) {
      if (row == dest.first && col == dest.second)
          return (true);
      else
          return (false);
  }

  // A Utility Function to calculate the 'h' heuristics.
  double calculateHValue(int row, int col, Pair dest) {
      // Return using the distance formula
      return ((double)sqrt ((row-dest.first)*(row-dest.first)
                            + (col-dest.second)*(col-dest.second)));
  }

  // A Utility Function to trace the path from the source
  // to destination
  void tracePath(mapCell mapCellDetails[][COL], Pair dest) {
      printf ("\nThe Path is ");
      int row = dest.first;
      int col = dest.second;

      stack<Pair> Path;

      while (!(mapCellDetails[row][col].parent_i == row
               && mapCellDetails[row][col].parent_j == col )) {
          Path.push (make_pair (row, col));
          int temp_row = mapCellDetails[row][col].parent_i;
          int temp_col = mapCellDetails[row][col].parent_j;
          row = temp_row;
          col = temp_col;
      }

      Path.push (make_pair (row, col));
      while (!Path.empty()) {
          pair<int,int> p = Path.top();
          Path.pop();
          printf("-> (%d,%d) ",p.first,p.second);
      }

      return;
  }

  // A Function to find the shortest path between
  // a given source mapCell to a destination mapCell according
  // to A* Search Algorithm
  void aStarSearch(int grid[][COL], Pair src, Pair dest) {
      // If the source is out of range
      if (isValid (src.first, src.second) == false) {
          printf ("Source is invalid\n");
          return;
      }

      // If the destination is out of range
      if (isValid (dest.first, dest.second) == false) {
          printf ("Destination is invalid\n");
          return;
      }

      // Either the source or the destination is blocked
      if (isUnBlocked(grid, src.first, src.second) == false ||
              isUnBlocked(grid, dest.first, dest.second) == false) {
          printf ("Source or the destination is blocked\n");
          return;
      }

      // If the destination mapCell is the same as source mapCell
      if (isDestination(src.first, src.second, dest) == true) {
          printf ("We are already at the destination\n");
          return;
      }

      // Create a closed list and initialise it to false which means
      // that no mapCell has been included yet
      // This closed list is implemented as a boolean 2D array
      bool closedList[ROW][COL];
      memset(closedList, false, sizeof (closedList));

      // Declare a 2D array of structure to hold the details
      //of that mapCell
      mapCell mapCellDetails[ROW][COL];

      int i, j;

      for (i=0; i<ROW; i++) {
          for (j=0; j<COL; j++) {
              mapCellDetails[i][j].f = FLT_MAX;
              mapCellDetails[i][j].g = FLT_MAX;
              mapCellDetails[i][j].h = FLT_MAX;
              mapCellDetails[i][j].parent_i = -1;
              mapCellDetails[i][j].parent_j = -1;
          }
      }

      // Initialising the parameters of the starting node
      i = src.first, j = src.second;
      mapCellDetails[i][j].f = 0.0;
      mapCellDetails[i][j].g = 0.0;
      mapCellDetails[i][j].h = 0.0;
      mapCellDetails[i][j].parent_i = i;
      mapCellDetails[i][j].parent_j = j;

      /*
       Create an open list having information as-
       <f, <i, j>>
       where f = g + h,
       and i, j are the row and column index of that mapCell
       Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
       This open list is implenented as a set of pair of pair.*/
      set<pPair> openList;

      // Put the starting mapCell on the open list and set its
      // 'f' as 0
      openList.insert(make_pair (0.0, make_pair (i, j)));

      // We set this boolean value as false as initially
      // the destination is not reached.
      bool foundDest = false;

      while (!openList.empty()) {
          pPair p = *openList.begin();

          // Remove this vertex from the open list
          openList.erase(openList.begin());

          // Add this vertex to the closed list
          i = p.second.first;
          j = p.second.second;
          closedList[i][j] = true;

          /*
          Generating all the 4 successor of this mapCell

                    N
                    |
                    |
              W----Cell----E
                    |
                    |
                    S

          Cell-->Popped Cell (i, j)
          N -->  North       (i-1, j)
          S -->  South       (i+1, j)
          E -->  East        (i, j+1)
          W -->  West           (i, j-1)
          */

          // To store the 'g', 'h' and 'f' of the 8 successors
          double gNew, hNew, fNew;

          //----------- 1st Successor (North) ------------

          // Only process this mapCell if this is a valid one
          if (isValid(i-1, j) == true) {
              // If the destination mapCell is the same as the
              // current successor
              if (isDestination(i-1, j, dest) == true) {
                  // Set the Parent of the destination mapCell
                  mapCellDetails[i-1][j].parent_i = i;
                  mapCellDetails[i-1][j].parent_j = j;
                  printf ("The destination mapCell is found\n");
                  tracePath (mapCellDetails, dest);
                  foundDest = true;
                  return;
              }
              // If the successor is already on the closed
              // list or if it is blocked, then ignore it.
              // Else do the following
              else if (closedList[i-1][j] == false &&
                       isUnBlocked(grid, i-1, j) == true) {
                  gNew = mapCellDetails[i][j].g + 1.0;
                  hNew = calculateHValue (i-1, j, dest);
                  fNew = gNew + hNew;

                  // If it isn’t on the open list, add it to
                  // the open list. Make the current square
                  // the parent of this square. Record the
                  // f, g, and h costs of the square mapCell
                  //                OR
                  // If it is on the open list already, check
                  // to see if this path to that square is better,
                  // using 'f' cost as the measure.
                  if (mapCellDetails[i-1][j].f == FLT_MAX ||
                          mapCellDetails[i-1][j].f > fNew) {
                      openList.insert( make_pair(fNew,
                                                 make_pair(i-1, j)));

                      // Update the details of this mapCell
                      mapCellDetails[i-1][j].f = fNew;
                      mapCellDetails[i-1][j].g = gNew;
                      mapCellDetails[i-1][j].h = hNew;
                      mapCellDetails[i-1][j].parent_i = i;
                      mapCellDetails[i-1][j].parent_j = j;
                  }
              }
          }

          //----------- 2nd Successor (South) ------------

          // Only process this mapCell if this is a valid one
          if (isValid(i+1, j) == true) {
              // If the destination mapCell is the same as the
              // current successor
              if (isDestination(i+1, j, dest) == true) {
                  // Set the Parent of the destination mapCell
                  mapCellDetails[i+1][j].parent_i = i;
                  mapCellDetails[i+1][j].parent_j = j;
                  printf("The destination mapCell is found\n");
                  tracePath(mapCellDetails, dest);
                  foundDest = true;
                  return;
              }
              // If the successor is already on the closed
              // list or if it is blocked, then ignore it.
              // Else do the following
              else if (closedList[i+1][j] == false &&
                       isUnBlocked(grid, i+1, j) == true) {
                  gNew = mapCellDetails[i][j].g + 1.0;
                  hNew = calculateHValue(i+1, j, dest);
                  fNew = gNew + hNew;

                  // If it isn’t on the open list, add it to
                  // the open list. Make the current square
                  // the parent of this square. Record the
                  // f, g, and h costs of the square mapCell
                  //                OR
                  // If it is on the open list already, check
                  // to see if this path to that square is better,
                  // using 'f' cost as the measure.
                  if (mapCellDetails[i+1][j].f == FLT_MAX ||
                          mapCellDetails[i+1][j].f > fNew) {
                      openList.insert( make_pair (fNew, make_pair (i+1, j)));
                      // Update the details of this mapCell
                      mapCellDetails[i+1][j].f = fNew;
                      mapCellDetails[i+1][j].g = gNew;
                      mapCellDetails[i+1][j].h = hNew;
                      mapCellDetails[i+1][j].parent_i = i;
                      mapCellDetails[i+1][j].parent_j = j;
                  }
              }
          }

          //----------- 3rd Successor (East) ------------

          // Only process this mapCell if this is a valid one
          if (isValid (i, j+1) == true) {
              // If the destination mapCell is the same as the
              // current successor
              if (isDestination(i, j+1, dest) == true) {
                  // Set the Parent of the destination mapCell
                  mapCellDetails[i][j+1].parent_i = i;
                  mapCellDetails[i][j+1].parent_j = j;
                  printf("The destination mapCell is found\n");
                  tracePath(mapCellDetails, dest);
                  foundDest = true;
                  return;
              }

              // If the successor is already on the closed
              // list or if it is blocked, then ignore it.
              // Else do the following
              else if (closedList[i][j+1] == false &&
                       isUnBlocked (grid, i, j+1) == true) {
                  gNew = mapCellDetails[i][j].g + 1.0;
                  hNew = calculateHValue (i, j+1, dest);
                  fNew = gNew + hNew;

                  // If it isn’t on the open list, add it to
                  // the open list. Make the current square
                  // the parent of this square. Record the
                  // f, g, and h costs of the square mapCell
                  //                OR
                  // If it is on the open list already, check
                  // to see if this path to that square is better,
                  // using 'f' cost as the measure.
                  if (mapCellDetails[i][j+1].f == FLT_MAX ||
                          mapCellDetails[i][j+1].f > fNew) {
                      openList.insert( make_pair(fNew,
                                          make_pair (i, j+1)));

                      // Update the details of this mapCell
                      mapCellDetails[i][j+1].f = fNew;
                      mapCellDetails[i][j+1].g = gNew;
                      mapCellDetails[i][j+1].h = hNew;
                      mapCellDetails[i][j+1].parent_i = i;
                      mapCellDetails[i][j+1].parent_j = j;
                  }
              }
          }

          //----------- 4th Successor (West) ------------

          // Only process this mapCell if this is a valid one
          if (isValid(i, j-1) == true) {
              // If the destination mapCell is the same as the
              // current successor
              if (isDestination(i, j-1, dest) == true) {
                  // Set the Parent of the destination mapCell
                  mapCellDetails[i][j-1].parent_i = i;
                  mapCellDetails[i][j-1].parent_j = j;
                  printf("The destination mapCell is found\n");
                  tracePath(mapCellDetails, dest);
                  foundDest = true;
                  return;
              }

              // If the successor is already on the closed
              // list or if it is blocked, then ignore it.
              // Else do the following
              else if (closedList[i][j-1] == false &&
                       isUnBlocked(grid, i, j-1) == true) {
                  gNew = mapCellDetails[i][j].g + 1.0;
                  hNew = calculateHValue(i, j-1, dest);
                  fNew = gNew + hNew;

                  // If it isn’t on the open list, add it to
                  // the open list. Make the current square
                  // the parent of this square. Record the
                  // f, g, and h costs of the square mapCell
                  //                OR
                  // If it is on the open list already, check
                  // to see if this path to that square is better,
                  // using 'f' cost as the measure.
                  if (mapCellDetails[i][j-1].f == FLT_MAX ||
                          mapCellDetails[i][j-1].f > fNew) {
                      openList.insert( make_pair (fNew,
                                            make_pair (i, j-1)));

                      // Update the details of this mapCell
                      mapCellDetails[i][j-1].f = fNew;
                      mapCellDetails[i][j-1].g = gNew;
                      mapCellDetails[i][j-1].h = hNew;
                      mapCellDetails[i][j-1].parent_i = i;
                      mapCellDetails[i][j-1].parent_j = j;
                  }
              }
          }

      } // end !openList.empty

      // When the destination mapCell is not found and the open
      // list is empty, then we conclude that we failed to
      // reach the destiantion mapCell. This may happen when the
      // there is no way to destination mapCell (due to blockages)
      if (foundDest == false)
          printf("Failed to find the Destination cell\n");

      return;
  } // end aStarSearch


  // Driver program to test above function
  // int main() {
  //     /* Description of the Grid-
  //      1--> The mapCell is not blocked
  //      0--> The mapCell is blocked    */
  //     int grid[ROW][COL] =
  //     {
  //         { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 },
  //         { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1 },
  //         { 1, 1, 1, 0, 1, 1, 0, 1, 0, 1 },
  //         { 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
  //         { 1, 1, 1, 0, 1, 1, 1, 0, 1, 0 },
  //         { 1, 0, 1, 1, 1, 1, 0, 1, 0, 0 },
  //         { 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
  //         { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 },
  //         { 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 }
  //     };
  //
  //     // Source is the left-most bottom-most corner
  //     Pair src = make_pair(8, 0);
  //
  //     // Destination is the left-most top-most corner
  //     Pair dest = make_pair(0, 0);
  //
  //     aStarSearch(grid, src, dest);
  //
  //     return(0);
  // }

} // namespace bdm


#endif
