
#ifndef GEOM_H_
#define GEOM_H_

#include "TGeometry.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "core/container/math_array.h"

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

// ---------------------------------------------------------------------------
  inline TGeoNode* GetNextNode(Double3 positionA, Double3 positionB) {
    TGeoNavigator *nav = gGeoManager->GetCurrentNavigator();
    if (!nav) nav = gGeoManager->AddNavigator();

    Double3 diffAB = GetDifAB(positionA, positionB);
    double distAB = GetDistance(diffAB);
    Double3 dABNorm = GetNormalisedDirection(distAB, diffAB);
    // Double3 to double [3] conversion
    double a[3]; double dAB[3];
    for (int i=0; i<3; ++i) {
      a[i] = positionA[i];
      dAB[i] = dABNorm[i];
    }
    // initialize starting point in the position of A
    // and starting direction shooting towards B
    nav->InitTrack(a, dAB);
    // Shoot the ray towards B and check if it hits something in between
    // ie find next node
    auto node = nav->FindNextBoundary();
    // std::cout << "We hit " << node->GetName() << std::endl;
    return node;
  } // end GetNextNode

// ---------------------------------------------------------------------------
  // return node distance from A, in direction A->B
  inline double DistToNode(Double3 positionA, Double3 dABNorm) {
    TGeoNavigator *nav = gGeoManager->GetCurrentNavigator();
    if (!nav) nav = gGeoManager->AddNavigator();

    // Double3 to double [3] conversion
    double a[3]; double dAB[3];
    for (int i=0; i<3; ++i) {
      a[i] = positionA[i];
      dAB[i] = dABNorm[i];
    }
    // initialize starting point in the position of A
    // and starting direction shooting towards B
    nav->InitTrack(a, dAB);
    // Shoot the ray towards B and check if it hits something in between
    // ie find next node
    nav->FindNextBoundary();
    // distance from positionA
    double step = nav->GetStep();

    return step;
  } // end DistToNode

// ---------------------------------------------------------------------------
  // return wall distance from A, in direction A->B
  inline double DistToWall(Double3 positionA, Double3 positionB) {
    Double3 dAB = GetDifAB(positionA, positionB);
    double distAB = GetDistance(dAB);
    Double3 dABNorm = GetNormalisedDirection(distAB, dAB);
    // distance from positionA
    double step = DistToNode(positionA, dABNorm);

    return step;
  } // end DistToWall

// ---------------------------------------------------------------------------
  // check if geom object exists between A and B
  inline bool ObjectInbetween(Double3 positionA, Double3 positionB) {
    Double3 dAB = GetDifAB(positionA, positionB);
    double distAB = GetDistance(dAB);
    Double3 dABNorm = GetNormalisedDirection(distAB, dAB);
    double step = DistToNode(positionA, dABNorm);
    if (step < distAB) {
      return true;
    }
    return false;
  } // end IsObjInbetween

// ---------------------------------------------------------------------------
  inline bool IsInsideStructure(Double3 position) {
    TGeoNavigator *nav = gGeoManager->GetCurrentNavigator();
    if (!nav) nav = gGeoManager->AddNavigator();

    TGeoNode* node = nav->FindNode(position[0], position[1], position[2]);
    std::string medium_name = node->GetMedium()->GetName();
    // std::cout << "Point "
    //           << position[0] << " " << position[1] << " " << position[2]
    //           << " is inside " << node->GetName()
    //           << " (" << node->GetMedium()->GetName() << ")" << std::endl;
    if (medium_name != "Air") {
      return true;
    }
    return false;
  } // end IsInsideStruct


// ---------------------------------------------------------------------------
  inline void ExportGeomToFoam() {
    auto* sim_space = gGeoManager->GetVolume("sim_space");
    // auto* sim_space_nodes = sim_space->GetNodes();
    double vert[24] = {0};
    double vert_master[3] = {0};

    for (int i = 0; i < sim_space->GetNdaughters(); ++i) {
      auto node = sim_space->GetNode(i);
      TGeoBBox *box = (TGeoBBox*)node->GetVolume()->GetShape();

      box->SetBoxPoints(&vert[0]); // copies the box vertices in local coordinates
      std::cout << node->GetName() << " vertices: ";
      for (auto point=0; point<8; point++) {
        // Convert each vertex to the reference frame of sim_space
        node->LocalToMaster(&vert[3*point], vert_master);
        std::cout << "{" << vert_master[0] << ", " << vert_master[1] << ", " << vert_master[2] << "}, ";
      }
      std::cout << "\n";

    } // end for node in sim_space

  } // end ExportGeomToFoam

}  // namespace bdm

  #endif // GEOM_H_
