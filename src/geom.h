
#ifndef GEOM_H_
#define GEOM_H_

#include "TGeometry.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "core/container/math_array.h"
#include "sim-param.h"

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
  inline void ExportGeomToFoam(std::string openFoamDir) {
    auto* sim = Simulation::GetActive();
    auto* param = sim->GetParam();
    double min_b = param->min_bound_;
    double max_b = param->max_bound_;

    auto* sim_space = gGeoManager->GetVolume("sim_space");

    // create file for export
    std::ofstream geometry_file;
    geometry_file.open(Concat(openFoamDir, "/system/blockMeshDict"));

    geometry_file << "FoamFile\n{\n"
      << "\tversion\t2.0;\n\tformat\tascii;\n"
      << "\tclass\tdictionary;\n\tobject\tblockMeshDict;\n"
      << "}\n"
      << "\nconvertToMeters 0.01;\n"
      << "\nvertices\n(\n"
      << "\t(" << min_b << " " << min_b << " " << min_b << ")\n"
      << "\t(" << max_b << " " << min_b << " " << min_b << ")\n"
      << "\t(" << max_b << " " << max_b << " " << min_b << ")\n"
      << "\t(" << min_b << " " << max_b << " " << min_b << ")\n"
      << "\t(" << min_b << " " << min_b << " " << max_b << ")\n"
      << "\t(" << max_b << " " << min_b << " " << max_b << ")\n"
      << "\t(" << max_b << " " << max_b << " " << max_b << ")\n"
      << "\t(" << min_b << " " << max_b << " " << max_b << ")\n";

      std::string boundaries;
      boundaries += Concat("\nboundary\n(\n"
        , "\tsim_space\n\t{\n"
        , "\t\ttype wall;\n"
        , "\t\tfaces\n\t\t(\n"
        , "\t\t\t(0 1 2 3)\n"
        , "\t\t\t(0 1 5 4)\n"
        , "\t\t\t(0 3 7 4)\n"
        , "\t\t\t(1 2 6 5)\n"
        , "\t\t\t(3 2 6 7)\n"
        , "\t\t\t(4 5 6 7)\n"
        , "\t\t);\n"
        , "\t}\n"
        , "\tboxes\n\t{\n"
        , "\t\ttype wall;\n"
        , "\t\tfaces\n");

    double vert[24] = {0};
    double vert_master[3] = {0};

    // for node in sim_space
    for (int i = 0; i < sim_space->GetNdaughters(); i++) {
      auto node = sim_space->GetNode(i);
      TGeoBBox *box = (TGeoBBox*)node->GetVolume()->GetShape();
      // copies the box vertices in local coordinates
      box->SetBoxPoints(&vert[0]);
      node->GetName(); // name of this box
      // for each vertex of this box
      for (auto point=0; point<8; point++) {
        // Convert each vertex to the reference frame of sim_space
        node->LocalToMaster(&vert[3*point], vert_master);
        // add vertex coordinates into vertices section
        geometry_file << "\t(" << vert_master[0] << " "
          << vert_master[1] << " " << vert_master[2] << ")\n";
      } // end for all vertices points
      // add each face of this box to boundary section
      boundaries += Concat("\t\t\t(vert1 vert2 vert3 vert4)\n"
        , "\t\t\t(vert1 vert2 vert3 vert4)\n"
        , "\t\t\t(vert1 vert2 vert3 vert4)\n"
        , "\t\t\t(vert1 vert2 vert3 vert4)\n"
        , "\t\t\t(vert1 vert2 vert3 vert4)\n"
        , "\t\t\t(vert1 vert2 vert3 vert4)\n");
    } // end for node in sim_space

    geometry_file << ");\n"
      << "\nblocks\n(\n"
      << "\thex (0 1 2 3 4 5 6 7) ("
      << max_b << " " << max_b << " " << max_b << ") simpleGrading (1 1 1)\n"
      << ");\n"; // end of blocks section

    boundaries += Concat( "\t\t);\n"
      , "\t}\n"
      , ");\n"); // end boundary section

    geometry_file << boundaries;
    geometry_file << "mergePatchPairs();";
    geometry_file.close();
  } // end ExportGeomToFoam

// ---------------------------------------------------------------------------
inline void ExportControlDict(std::string openFoamDir) {
  // create file for export
  std::ofstream geometry_control;
   geometry_control.open(Concat(openFoamDir, "/system/controlDict"));

   geometry_control << "FoamFile\n{\n"
     << "\tversion\t2.0;\n\tformat\tascii;\n"
     << "\tclass\tdictionary;\n\tobject\tcontrolDict;\n"
     << "}\n"
     << "\napplication\tnavigationFoam;\n"
     << "startFrom\tlatestTime;\n"
     << "startTime\t0;\n"
     << "stopAt\tendTime;\n"
     << "endTime\t10;\n"
     << "deltaT\t0.005;\n"
     << "writeControl\trunTime;\n"
     << "writeInterval\t0.1;\n"
     << "purgeWrite\t0;\n"
     << "writeFormat\tascii;\n"
     << "writePrecision\t3;\n"
     << "writeCompression\toff;\n"
     << "timeFormat\tgeneral;\n"
     << "timePrecision\t3;\n"
     << "runTimeModifiable\ttrue;\n";

     geometry_control.close();
  }

// ---------------------------------------------------------------------------
  inline void ExportFoamFiles(std::string openFoamDir) {
    // create openFoam directories
    if (std::system(Concat("mkdir ", openFoamDir).c_str())) {
      std::cout << "couldn't create openFOAM directory for files export"
                << std::endl;
    }
    if (std::system(Concat("mkdir ", openFoamDir, "/system/").c_str())) {
      std::cout << "couldn't create openFOAM/system directory"
               << std::endl;
    };

    ExportControlDict(openFoamDir);
    // export ROOT geometry to OpenFOAM format
    ExportGeomToFoam(openFoamDir);

    std::cout << "OpenFoam files created" << std::endl;
  } // end ExportFoamFiles


}  // namespace bdm

  #endif // GEOM_H_
