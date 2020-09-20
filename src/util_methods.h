// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef UTILS_METHODS_
#define UTILS_METHODS_

#include "biodynamo.h"

namespace bdm {

// ------------------------------------------------------------------------
  inline float TriangleArea(double x1, double y1, double x2, double y2, double x3, double y3) {
     return std::abs((x1*(y2-y3) + x2*(y3-y1)+ x3*(y1-y2))/2.0);
  } // end TriangleArea

// ------------------------------------------------------------------------
  inline bool IsInsideTriangle(Double3 tri_a, Double3 tri_b, Double3 tri_c, Double3 point) {
    float a = TriangleArea(tri_a[0], tri_a[1], tri_b[0], tri_b[1], tri_c[0], tri_c[1]);
    float a1 = TriangleArea(point[0], point[1], tri_b[0], tri_b[1], tri_c[0], tri_c[1]);
    float a2 = TriangleArea(tri_a[0], tri_a[1], point[0], point[1], tri_c[0], tri_c[1]);
    float a3 = TriangleArea(tri_a[0], tri_a[1], tri_b[0], tri_b[1], point[0], point[1]);
    return (a == a1 + a2 + a3);
  } // end IsInsideTriangle

// ------------------------------------------------------------------------
//   inline double GetDistance(Double3 dAB) {
//     return std::sqrt(dAB[0]*dAB[0] + dAB[1]*dAB[1] + dAB[2]*dAB[2]);
//   } // end GetDistance
//
// // ---------------------------------------------------------------------------
//   inline Double3 GetDifAB(Double3 positionA, Double3 positionB) {
//     Double3 dAB;
//     for (int i=0; i<3; ++i) {
//       dAB[i] = positionB[i] - positionA[i];
//     }
//     return dAB;
//   } // end GetDifAB
//
// // ---------------------------------------------------------------------------
//   inline Double3 GetNormalisedDirection(double distAB, Double3 dAB) {
//   // normalize the direction
//     for (int i=0; i<3; ++i) {
//       dAB[i] /= distAB;
//     }
//     return dAB;
//   } // end GetNormalisedDirection
//

}

#endif // UTILS_METHODS_
