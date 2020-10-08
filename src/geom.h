
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
  inline void ExportGeomToFoam() {
    auto* sim = Simulation::GetActive();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();

    auto* sim_space = gGeoManager->GetVolume("sim_space");

    double min_b = param->min_bound_;
    double max_b = param->max_bound_;

    // create file for export
    std::ofstream geometry_file;
    geometry_file.open(
      Concat(param->output_dir_, "/blockMeshDict"));

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
      << "\t(" << min_b << " " << max_b << " " << max_b << ")\n"
      << ");\n"
      << "\nblocks\n("
      << "\n\thex (0 1 2 3 4 5 6 7) ("
      << max_b << " " << max_b << " " << max_b << ") simpleGrading (1 1 1)\n"
      << ");\n"
      << "\ngeometry\n{\n";

    double vert[24] = {0};
    double vert_master[3] = {0};

    // for node in sim_space
    for (int i = 0; i < sim_space->GetNdaughters(); ++i) {
      auto node = sim_space->GetNode(i);
      TGeoBBox *box = (TGeoBBox*)node->GetVolume()->GetShape();
      // copies the box vertices in local coordinates
      box->SetBoxPoints(&vert[0]);

      double min_vert[3] = {max_b, max_b, max_b};
      double max_vert[3] = {min_b, min_b, min_b};
      for (auto point=0; point<8; point++) {
        // Convert each vertex to the reference frame of sim_space
        node->LocalToMaster(&vert[3*point], vert_master);
        if (vert_master[0] < min_vert[0] &&
          vert_master[1] < min_vert[1] &&
          vert_master[2] < min_vert[2]) {
          min_vert[0] = vert_master[0];
          min_vert[1] = vert_master[1];
          min_vert[2] = vert_master[2];
        }
        if (vert_master[0] > max_vert[0] &&
          vert_master[1] > max_vert[1] &&
          vert_master[2] > max_vert[2]) {
          max_vert[0] = vert_master[0];
          max_vert[1] = vert_master[1];
          max_vert[2] = vert_master[2];
        }
      } // end for all vertices points
      // write corresponding object in Foam file
      geometry_file << "\tbox\n\t{\n"
        << "\t\ttype\tsearchableBox;\n"
        << "\t\tmin\t(" << min_vert[0] << " "
        << min_vert[1] << " " << min_vert[2] << ");\n"
        << "\t\tmax\t(" << max_vert[0] << " "
        << max_vert[1] << " " << max_vert[2] << ");\n"
        << "\t}\n";
    } // end for node in sim_space

    geometry_file << "};";
    geometry_file.close();
    std::cout << "geometry exported to Foam format" << std::endl;
  } // end ExportGeomToFoam

}  // namespace bdm

  #endif // GEOM_H_
