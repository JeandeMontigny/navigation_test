// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef GEOM_H_
#define GEOM_H_

#include "TGeometry.h"
#include "TGeoManager.h"
#include "util_methods.h"

namespace bdm {

  inline TGeoManager* BuildMaze() {

    TGeoManager *geom = new TGeoManager("maze", "geometry test for agent navigation");

    // materials
    TGeoMaterial *Vacuum = new TGeoMaterial("vacuum", 0, 0, 0);
    TGeoMaterial *Fe = new TGeoMaterial("Fe",55.845,26,7.87);
    // media
    TGeoMedium *Air = new TGeoMedium("Air", 0, Vacuum);
    TGeoMedium *Iron = new TGeoMedium("Iron", 0, Fe);

    // simulation  volume - 2m * 2m * 2m
    TGeoVolume *sim_space = gGeoManager->MakeBox("sim_space", Air, 150, 150, 100);
    gGeoManager->SetTopVolume(sim_space);
    gGeoManager->SetTopVisible(0);
    TGeoVolume *mBlocks;

    // maze - 2m * 2m * 2m
    mBlocks = geom->MakeBox("floor_roof", Iron, 150, 100, 4);
    mBlocks->SetLineColor(kBlack);
    sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(0, 0, -100));
    sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(0, 0, 100));

    mBlocks = geom->MakeBox("wall_length", Iron, 150, 4, 100);
    sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(0, -100, 0));
    sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(0, 100, 0));

    mBlocks = geom->MakeBox("wall_width", Iron, 4, 100, 100);
    sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(-150, 0, 0));
    sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(150, 0, 0));

    // // inside wall
    mBlocks = geom->MakeBox("inside_wall", Iron, 4, 65, 100);
    sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(50, 35, 0));
    sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(-50, -35, 0));

    // close geometry
    geom->CloseGeometry();

    std::cout << "geom construction done" << std::endl;

    // set max threads
    // compilation error when using ThreadInfo:
    // /usr/bin/ld: CMakeFiles/epidemio_2-room.dir/src/epidemio_2-room.cc.o: undefined reference to symbol 'numa_num_configured_nodes@@libnuma_1.2'
    // gGeoManager->SetMaxThreads(ThreadInfo::GetInstance()->GetMaxThreads());
    gGeoManager->SetMaxThreads(4);

    // export geom to gdml file
    geom->Export("navigation.gdml");

    return geom;

  } // end BuildTwoRoom

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

}  // namespace bdm

#endif // GEOM_H_
