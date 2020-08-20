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

  inline TGeoManager* BuildSupermarket() {

      TGeoManager *geom = new TGeoManager("supermarket", "supermarket scenario: 50m * 30m * 4m");

      // materials
      TGeoMaterial *Vacuum = new TGeoMaterial("vacuum", 0, 0, 0);
      TGeoMaterial *Fe = new TGeoMaterial("Fe",55.845,26,7.87);
      TGeoMaterial *Cement = new TGeoMaterial("Cement",55.845,26,7.87);
      // media
      TGeoMedium *Air = new TGeoMedium("Air", 0, Vacuum);
      TGeoMedium *Concrete = new TGeoMedium("concrete", 0, Cement);
      TGeoMedium *Iron = new TGeoMedium("Iron", 0, Fe);

      // simulation volume - 50m * 30m * 4m
      TGeoVolume *sim_space = gGeoManager->MakeBox("sim_space", Air, 2500, 2500, 2500);
      gGeoManager->SetTopVolume(sim_space);
      gGeoManager->SetTopVisible(0);


      TGeoVolume *mBlocks;

      // supermarket - 50m * 30m * 4m
      // outside walls
      mBlocks = geom->MakeBox("floor_roof", Concrete, 2500, 1500, 5);
      mBlocks->SetLineColor(kBlack);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(0, 0, -200));
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(0, 0, 200));
      // length side wall
      mBlocks = geom->MakeBox("wall_length", Concrete, 2500, 5, 200);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(0, 1500, 0));
      // length entrance side
      mBlocks = geom->MakeBox("wall_length_entrance_long", Concrete, 1000, 5, 200);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(0, -1500, 0));
      mBlocks = geom->MakeBox("wall_length_entrance_short", Concrete, 500, 5, 200);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(-2000, -1500, 0));
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(2000, -1500, 0));
      // width side wall
      mBlocks = geom->MakeBox("wall_width", Concrete, 5, 1500, 200);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(2500, 0, 0));
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(-2500, 0, 0));

      // inside
      // central shelves (y orientation)
      int shelf_per_row = 4;
      int nb_shelf_line = 7;
      for (int x_shelf = 0; x_shelf < nb_shelf_line; x_shelf++) {
        int x_position = x_shelf*350 - nb_shelf_line * 150;
        for (int y_shelf = 0; y_shelf < shelf_per_row; y_shelf++) {
          int y_position = y_shelf*401 - 250;
          mBlocks = geom->MakeBox("shelf_center", Iron, 1, 200, 125);
          sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(x_position, y_position, -75));
          for (int i = 0; i < 9 ; i++) {
            int shelf_height = i*25 - 190;
            mBlocks = geom->MakeBox("shelf", Iron, 50, 200, 1);
            sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(x_position, y_position, shelf_height));
          }
        }
      }
      // left side shelves (x orientation)
      shelf_per_row = 2;
      nb_shelf_line = 6;
      for (int x_shelf = 0; x_shelf < shelf_per_row; x_shelf++) {
        int x_position = x_shelf*401 - 2250;
        for (int y_shelf = 0; y_shelf < nb_shelf_line; y_shelf++) {
          int y_position = y_shelf*350 - 600;
          mBlocks = geom->MakeBox("shelf_left_side", Iron, 200, 1, 125);
          sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(x_position, y_position, -75));
          for (int i = 0; i < 9 ; i++) {
            int shelf_height = i*25 - 200;
            mBlocks = geom->MakeBox("shelf", Iron, 200, 50, 1);
            sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(x_position, y_position, shelf_height));
          }
        }
      }
      // right side shelves (x orientation)
      shelf_per_row = 2;
      nb_shelf_line = 6;
      for (int x_shelf = 0; x_shelf < shelf_per_row; x_shelf++) {
        int x_position = x_shelf*401 + 1850;
        for (int y_shelf = 0; y_shelf < nb_shelf_line; y_shelf++) {
          int y_position = y_shelf*350 - 600;
          mBlocks = geom->MakeBox("shelf_right_side", Iron, 200, 1, 125);
          sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(x_position, y_position, -75));
          for (int i = 0; i < 9 ; i++) {
            int shelf_height = i*25 - 200;
            mBlocks = geom->MakeBox("shelf", Iron, 200, 50, 1);
            sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(x_position, y_position, shelf_height));
          }
        }
      }

      // supermarket checkout (y orientation)
      int nb_checkout = 5;
      for (int x_checkout = 0; x_checkout < nb_checkout; x_checkout++) {
        int x_position = x_checkout*500 - 1250;
        mBlocks = geom->MakeBox("checkout", Iron, 50, 110, 50);
        sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(x_position, -1000, -125));
        sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(x_position+150, -1000, -125));
      }

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
