// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef GEOM_CONSTRUCT_H_
#define GEOM_CONSTRUCT_H_

#include "geom.h"
#include "TGeometry.h"
#include "TGeoManager.h"
#include "util_methods.h"

namespace bdm {

  inline TGeoManager* BuildBus() {

      TGeoManager *geom = new TGeoManager("bus", "public transport 1");

      // materials
      TGeoMaterial *Vacuum = new TGeoMaterial("vacuum", 0, 0, 0);
      TGeoMaterial *Fe = new TGeoMaterial("Fe",55.845,26,7.87);
      // media
      TGeoMedium *Air = new TGeoMedium("Air", 0, Vacuum);
      TGeoMedium *Iron = new TGeoMedium("Iron", 0, Fe);

      // simulation volume - 12m * 3m * 3.5m
      TGeoVolume *sim_space = gGeoManager->MakeBox("sim_space", Air, 650, 650, 650);
      gGeoManager->SetTopVolume(sim_space);
      gGeoManager->SetTopVisible(0);


      TGeoVolume *mBlocks;

      // bus - 11m * 2.5m * 3m
      // floor and roof
      mBlocks = geom->MakeBox("floor_roof", Iron, 550, 125, 1);
      mBlocks->SetLineColor(kBlack);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(0, 0, -125));
      sim_space->AddNodeOverlap(mBlocks, 2, new TGeoTranslation(0, 0, 125));

      // left side
      mBlocks = geom->MakeBox("left_side", Iron, 550, 1, 125);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(0, -125, 0));
      // right side
      mBlocks = geom->MakeBox("back_right_side", Iron, 150, 1, 125);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(400, 125, 0));
      mBlocks = geom->MakeBox("middle_right_side", Iron, 285, 1, 125);
      sim_space->AddNodeOverlap(mBlocks, 2, new TGeoTranslation(-135, 125, 0));
      mBlocks = geom->MakeBox("front_right_side", Iron, 15, 1, 125);
      sim_space->AddNodeOverlap(mBlocks, 3, new TGeoTranslation(-535, 125, 0));
      // right side top
      mBlocks = geom->MakeBox("right_side_top", Iron, 50, 1, 25);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(-470, 125, 100));
      sim_space->AddNodeOverlap(mBlocks, 2, new TGeoTranslation(200, 125, 100));
      // right side doors
      mBlocks = geom->MakeBox("right_side_top", Iron, 50, 1, 100);
      sim_space->AddNodeOverlap(mBlocks, 3, new TGeoTranslation(-470, 125, -25));
      sim_space->AddNodeOverlap(mBlocks, 4, new TGeoTranslation(200, 125, -25));

      // back and front
      mBlocks = geom->MakeBox("back_front", Iron, 1, 125, 125);
      mBlocks->SetLineColor(kBlack);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(550, 0, 0));
      sim_space->AddNodeOverlap(mBlocks, 2, new TGeoTranslation(-550, 0, 0));

      // driver side
      mBlocks = geom->MakeBox("driver_side", Iron, 62, 1, 50);
      mBlocks->SetLineColor(kBlack);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(-488, -10, -75));
      // driver back
      mBlocks = geom->MakeBox("driver_back", Iron, 1, 60, 125);
      mBlocks->SetLineColor(kBlack);
      sim_space->AddNodeOverlap(mBlocks, 1, new TGeoTranslation(-425, -70, 0));

      // seats
      int seat_i = 0;
      int seat_j = 0;
      mBlocks = geom->MakeBox("seats", Air, 20, 20, 20);
      mBlocks->SetLineColor(kGray);
      TGeoVolume *mBlocks_back = geom->MakeBox("seat_back", Iron, 2, 20, 50);
      mBlocks_back->SetLineColor(kBlack);
      // driver seat
      sim_space->AddNodeOverlap(mBlocks, ++seat_i, new TGeoTranslation(-475, -65, -100));
      for (int j = -5; j < 7; j++) {
        int x_position = j*75+45;
        // left rows
        sim_space->AddNodeOverlap(mBlocks, ++seat_i, new TGeoTranslation(x_position, -95, -100));
        sim_space->AddNodeOverlap(mBlocks, ++seat_i, new TGeoTranslation(x_position, -50, -100));
        //seat back
        sim_space->AddNodeOverlap(mBlocks_back, ++seat_j, new TGeoTranslation(x_position+20, -95, -75));
        sim_space->AddNodeOverlap(mBlocks_back, ++seat_j, new TGeoTranslation(x_position+20, -50, -75));
        // right rows
        if (j!=2 && j!=3) {
          sim_space->AddNodeOverlap(mBlocks, ++seat_i, new TGeoTranslation(x_position, 95, -100));
          sim_space->AddNodeOverlap(mBlocks, ++seat_i, new TGeoTranslation(x_position, 50, -100));
          sim_space->AddNodeOverlap(mBlocks_back, ++seat_j, new TGeoTranslation(x_position+20, 95, -75));
          sim_space->AddNodeOverlap(mBlocks_back, ++seat_j, new TGeoTranslation(x_position+20, 50, -75));
        }
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
    // geom->Export("navigation.gdml");

    return geom;

  } // end BuildMaze

}  // namespace bdm

#endif // GEOM_CONSTRUCT_H_
