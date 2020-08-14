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

  inline double GetDistance(Double3 dAB) {
    return std::sqrt(dAB[0]*dAB[0] + dAB[1]*dAB[1] + dAB[2]*dAB[2]);
  } // end GetDistance

// ---------------------------------------------------------------------------
  inline Double3 GetDifAB(Double3 positionA, Double3 positionB) {
    Double3 dAB;
    for (int i=0; i<3; ++i) {
      dAB[i] = positionB[i] - positionA[i];
    }
    return dAB;
  } // end GetDifAB

// ---------------------------------------------------------------------------
  inline Double3 GetNormalisedDirection(double distAB, Double3 dAB) {
  // normalize the direction
    for (int i=0; i<3; ++i) {
      dAB[i] /= distAB;
    }
    return dAB;
  } // end GetNormalisedDirection


}

#endif // UTILS_METHODS_
