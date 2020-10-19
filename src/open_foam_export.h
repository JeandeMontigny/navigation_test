
#ifndef OPEN_FOAM_EXPORT_H_
#define OPEN_FOAM_EXPORT_H_

#include "TGeometry.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "core/container/math_array.h"
#include "sim-param.h"
#include "biodynamo.h"
#include "population_creation_utils.h"

namespace bdm {

// ---------------------------------------------------------------------------
  inline void ExportGeomToFoam(std::string openFoamDir) {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    double min_b = param->min_bound_;
    double max_b = param->max_bound_;

    auto* sim_space = gGeoManager->GetVolume("sim_space");

    // create file for export
    std::ofstream geometry_file;
    geometry_file.open(Concat(openFoamDir, "system/blockMeshDict"));

    // TODO: add agents (if any) as spheres
    //       add their mouth position and direction
    //       for breathing/sneezing

    auto agents_position = GetAgentsPositionList();
    auto agents_direction = GetAgentsDirectionList();

    std::string agents_geometry;
    std::string agents_vertices;
    std::string agents_blocks;
    std::string agents_edges;
    std::string agents_faces;
    std::string agents_boundaries;

    agents_geometry += Concat("geometry\n{");

    // for (size_t agent = 0; agent < agents_position.size(); agent++ ) {
    //   Double3 pos = agents_position[agent];
    // }

/* using geometry:

    geometry
    {
        sphere
        {
            type searchableSphere;
            centre (0 0 0);
            radius 1;
        }
    }

    scale 1;

    n    10;

    v    0.5773502;
    mv  -0.5773502;
    vh   0.2886751;
    mvh -0.2886751;

    a    0.7071067;
    ma  -0.7071067;
    ah   0.3464101;
    mah -0.3464101;

    vertices
    (
        ($mvh $mvh $mvh)
        ( $vh $mvh $mvh)
        ( $vh  $vh $mvh)
        ($mvh  $vh $mvh)
        ($mvh $mvh  $vh)
        ( $vh $mvh  $vh)
        ( $vh  $vh  $vh)
        ($mvh  $vh  $vh)

        ($mv $mv $mv)
        ( $v $mv $mv)
        ( $v  $v $mv)
        ($mv  $v $mv)
        ($mv $mv  $v)
        ( $v $mv  $v)
        ( $v  $v  $v)
        ($mv  $v  $v)
    );

    blocks
    (
        hex ( 0  1  2  3  4  5  6  7) ($n $n $n) simpleGrading (1 1 1)
        hex ( 9  8 12 13  1  0  4  5) ($n $n $n) simpleGrading (1 1 1)
        hex (10  9 13 14  2  1  5  6) ($n $n $n) simpleGrading (1 1 1)
        hex (11 10 14 15  3  2  6  7) ($n $n $n) simpleGrading (1 1 1)
        hex ( 8 11 15 12  0  3  7  4) ($n $n $n) simpleGrading (1 1 1)
        hex ( 8  9 10 11  0  1  2  3) ($n $n $n) simpleGrading (1 1 1)
        hex (13 12 15 14  5  4  7  6) ($n $n $n) simpleGrading (1 1 1)
    );

    edges
    (
        arc  8  9 (0 $ma $ma)
        arc 10 11 (0 $a $ma)
        arc 14 15 (0 $a $a)
        arc 12 13 (0 $ma $a)

        arc  8 11 ($ma 0 $ma)
        arc  9 10 ($a 0 $ma)
        arc 13 14 ($a 0 $a)
        arc 12 15 ($ma 0 $a)

        arc  8 12 ($ma $ma 0)
        arc  9 13 ($a $ma 0)
        arc 10 14 ($a $a 0)
        arc 11 15 ($ma $a 0)


        arc  0  1 (0 $mah $mah)
        arc  2  3 (0 $ah $mah)
        arc  6  7 (0 $ah $ah)
        arc  4  5 (0 $mah $ah)

        arc  0  3 ($mah 0 $mah)
        arc  1  2 ($ah 0 $mah)
        arc  5  6 ($ah 0 $ah)
        arc  4  7 ($mah 0 $ah)

        arc  0  4 ($mah $mah 0)
        arc  1  5 ($ah $mah 0)
        arc  2  6 ($ah $ah 0)
        arc  3  7 ($mah $ah 0)
    );

    faces
    (
        project ( 8 12 15 11) sphere
        project (10 14 13  9) sphere
        project ( 9 13 12  8) sphere
        project (11 15 14 10) sphere
        project ( 8 11 10  9) sphere
        project (12 13 14 15) sphere
    );

    boundary
    (
        walls
        {
            type wall;
            faces
            (
                ( 8 12 15 11)
                (10 14 13  9)
                ( 9 13 12  8)
                (11 15 14 10)
                ( 8 11 10  9)
                (12 13 14 15)
            );
        }
    );
*/

    // TODO: get min and max x, y, z coord of simu
    geometry_file << "FoamFile\n{\n"
      << "\tversion\t2.0;\n\tformat\tascii;\n"
      << "\tclass\tdictionary;\n\tobject\tblockMeshDict;\n"
      << "}\n"
      << "\nconvertToMeters 0.01;\n"
      << "\nvertices\n(\n"
      << "\t(" << -550 << " " << -126 << " " << -126 << ")\n"
      << "\t(" << 550 << " " << -126 << " " << -126 << ")\n"
      << "\t(" << 550 << " " << 126 << " " << -126 << ")\n"
      << "\t(" << -550 << " " << 126 << " " << -126 << ")\n"
      << "\t(" << -550 << " " << -126 << " " << 126 << ")\n"
      << "\t(" << 550 << " " << -126 << " " << 126 << ")\n"
      << "\t(" << 550 << " " << 126 << " " << 126 << ")\n"
      << "\t(" << -550 << " " << 126 << " " << 126 << ")\n";

      std::string blocks;
      blocks += Concat("\nblocks\n(\n"
        , "\thex (0 1 2 3 4 5 6 7) ("
        , max_b, " ", max_b, " ", max_b, ") simpleGrading (1 1 1)\n");

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
        , "\t\tfaces\n"
        , "\t\t(\n");

    double vert[24] = {0};
    double vert_master[8][3];
    // for node in sim_space
    for (int i = 0; i < sim_space->GetNdaughters(); i++) {
      auto node = sim_space->GetNode(i);
      TGeoBBox *box = (TGeoBBox*)node->GetVolume()->GetShape();

      double min_x = max_b; double max_x = min_b;
      double min_y = max_b; double max_y = min_b;
      double min_z = max_b; double max_z = min_b;
      // copies the box vertices in local coordinates
      box->SetBoxPoints(&vert[0]);
      node->GetName(); // name of this box
      // for each vertex of this box
      for (auto point = 0; point < 8; point++) {
        // Convert each vertex to the reference frame of sim_space
        node->LocalToMaster(&vert[3*point], vert_master[point]);
        // find min and max values for x, y, z
        if (vert_master[point][0] > max_x) {
          max_x = vert_master[point][0];
        }
        if (vert_master[point][0] < min_x) {
          min_x = vert_master[point][0];
        }
        if (vert_master[point][1] > max_y) {
          max_y = vert_master[point][1];
        }
        if (vert_master[point][1] < min_y) {
          min_y = vert_master[point][1];
        }
        if (vert_master[point][2] > max_z) {
          max_z = vert_master[point][2];
        }
        if (vert_master[point][2] < min_z) {
          min_z = vert_master[point][2];
        }
      } // end for all vertices points

      // sort vert_master
      // - - - // vertice 0
      // + - - // vertice 1
      // + + - // vertice 2
      // - + - // vertice 3
      // - - + // vertice 4
      // + - + // vertice 5
      // + + + // vertice 6
      // - + + // vertice 7
      double vert_master_sorted[8][3];
      // TODO: cleaner way
      for (auto vert = 0; vert < 8; vert++) {
        if (vert_master[vert][0] == min_x && vert_master[vert][1] == min_y
          && vert_master[vert][2] == min_z) {
            vert_master_sorted[0][0] = vert_master[vert][0];
            vert_master_sorted[0][1] = vert_master[vert][1];
            vert_master_sorted[0][2] = vert_master[vert][2];
        }
        if (vert_master[vert][0] == max_x && vert_master[vert][1] == min_y
          && vert_master[vert][2] == min_z) {
            vert_master_sorted[1][0] = vert_master[vert][0];
            vert_master_sorted[1][1] = vert_master[vert][1];
            vert_master_sorted[1][2] = vert_master[vert][2];
        }
        if (vert_master[vert][0] == max_x && vert_master[vert][1] == max_y
          && vert_master[vert][2] == min_z) {
            vert_master_sorted[2][0] = vert_master[vert][0];
            vert_master_sorted[2][1] = vert_master[vert][1];
            vert_master_sorted[2][2] = vert_master[vert][2];
        }
        if (vert_master[vert][0] == min_x && vert_master[vert][1] == max_y
          && vert_master[vert][2] == min_z) {
            vert_master_sorted[3][0] = vert_master[vert][0];
            vert_master_sorted[3][1] = vert_master[vert][1];
            vert_master_sorted[3][2] = vert_master[vert][2];
        }
        if (vert_master[vert][0] == min_x && vert_master[vert][1] == min_y
          && vert_master[vert][2] == max_z) {
            vert_master_sorted[4][0] = vert_master[vert][0];
            vert_master_sorted[4][1] = vert_master[vert][1];
            vert_master_sorted[4][2] = vert_master[vert][2];
        }
        if (vert_master[vert][0] == max_x && vert_master[vert][1] == min_y
          && vert_master[vert][2] == max_z) {
            vert_master_sorted[5][0] = vert_master[vert][0];
            vert_master_sorted[5][1] = vert_master[vert][1];
            vert_master_sorted[5][2] = vert_master[vert][2];
        }
        if (vert_master[vert][0] == max_x && vert_master[vert][1] == max_y
          && vert_master[vert][2] == max_z) {
            vert_master_sorted[6][0] = vert_master[vert][0];
            vert_master_sorted[6][1] = vert_master[vert][1];
            vert_master_sorted[6][2] = vert_master[vert][2];
        }
        if (vert_master[vert][0] == min_x && vert_master[vert][1] == max_y
          && vert_master[vert][2] == max_z) {
            vert_master_sorted[7][0] = vert_master[vert][0];
            vert_master_sorted[7][1] = vert_master[vert][1];
            vert_master_sorted[7][2] = vert_master[vert][2];
        }
      }

      for (int vert_i = 0; vert_i < 8; vert_i++) {
        // add vertex coordinates into vertices section
        geometry_file << "\t(" << vert_master_sorted[vert_i][0] << " "
          << vert_master_sorted[vert_i][1] << " "
          << vert_master_sorted[vert_i][2] << ")\n";
      } // end write each vertice of this box
      // add each vertex of this box to blocks section
      blocks += Concat ("\thex (",i*8+8, " ", i*8+9, " ", i*8+10, " "
        , i*8+11, " ", i*8+12, " ", i*8+13, " ", i*8+14, " ", i*8+15
        , ") (1 1 1) simpleGrading (1 1 1)\n");
      // add each face of this box to boundary section
      boundaries += Concat(
          "\t\t\t(", i*8+8, " ", i*8+9, " ", i*8+10, " ", i*8+11, ")\n"
        , "\t\t\t(", i*8+8, " ", i*8+9, " ", i*8+13, " ", i*8+12, ")\n"
        , "\t\t\t(", i*8+8, " ", i*8+11, " ", i*8+15, " ", i*8+12, ")\n"
        , "\t\t\t(", i*8+9, " ", i*8+10, " ", i*8+14, " ", i*8+13, ")\n"
        , "\t\t\t(", i*8+11, " ", i*8+10, " ", i*8+14, " ", i*8+15, ")\n"
        , "\t\t\t(", i*8+12, " ", i*8+13, " ", i*8+14, " ", i*8+15, ")\n");
    } // end for node in sim_space

    geometry_file << ");\n"; // end vertices section

    geometry_file << blocks << ");\n"; // end of blocks section

    boundaries += Concat("\t\t);\n"
      , "\t}\n"
      , ");\n"); // end boundary section

    geometry_file << boundaries;
    geometry_file << "mergePatchPairs();";
    geometry_file.close();
  } // end ExportGeomToFoam

// ---------------------------------------------------------------------------
inline void ExportControlDict(std::string openFoamDir) {
  // create file for export
  std::ofstream control_file;
  control_file.open(Concat(openFoamDir, "system/controlDict"));

  control_file << "FoamFile\n{\n"
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

    control_file.close();
  }

// ---------------------------------------------------------------------------
  inline void ExportFvSchenes(std::string openFoamDir) {
    // create file for export
    std::ofstream schemes_file;
    schemes_file.open(Concat(openFoamDir, "system/fvSchemes"));

    schemes_file << "FoamFile\n{\n"
      << "\tversion\t2.0;\n\tformat\tascii;\n"
      << "\tclass\tdictionary;\n\tobject\tfvSchemes;\n"
      << "}\n"
      << "\nddtSchemes\n{\n"
      << "\tdefault\tEuler;\n"
      << "}\n"
      << "\ngradSchemes\n{\n"
      << "\tdefault\tGauss linear;\n"
      << "\tgrad(T)\tGauss linear;\n"
      << "}\n"
      << "\ndivSchemes\n{\n"
      << "\tdefault\tnone;\n"
      << "}\n";

      schemes_file.close();
  }

// ---------------------------------------------------------------------------
  inline void ExportFvSolution(std::string openFoamDir) {
    // create file for export
    std::ofstream solution_file;
    solution_file.open(Concat(openFoamDir, "system/fvSolution"));

    solution_file << "FoamFile\n{\n"
      << "\tversion\t2.0;\n\tformat\tascii;\n"
      << "\tclass\tdictionary;\n\tobject\tfvSolution;\n"
      << "}\n"
      << "\nsolvers\n{\n"
      << "\tT\n\t{\n"
      << "\t\tsolver\tPCG;\n"
      << "\t\tpreconditioner\tDIC;\n"
      << "\t\ttolerance\t1e-6;\n"
      << "\t\trelTol\t0;\n"
      << "\t}\n"
      << "}\n"
      << "\nSIMPLE\n{\n"
      << "\tnNonOrthogonalCorrectors\t2;\n"
      << "}";

      solution_file.close();
  }

// ---------------------------------------------------------------------------
  inline void ExportFoamFiles(std::string openFoamDir) {
    // create openFoam directories
    if (std::system(Concat("mkdir ", openFoamDir).c_str())) {    // std::vector<Double3> agents_position_list;
    //
    // auto get_agents_position_list = [&agents_position_list](SimObject* so) {
    //   auto* hu = bdm_static_cast<Human*>(so);
    //   agents_position_list.push_back(hu->GetPosition());
    // };
    // rm->ApplyOnAllElements(get_agents_position_list);
      std::cout << "couldn't create openFOAM directory for files export"
                << std::endl;
    }
    if (std::system(Concat("mkdir ", openFoamDir, "system/").c_str())) {
      std::cout << "couldn't create openFOAM/system directory"
               << std::endl;
    };

    // create empty ai.foam for paraview visualisation
    std::ofstream visu_file;
    visu_file.open(Concat(openFoamDir, "ai.foam"));
    visu_file.close();

    ExportControlDict(openFoamDir);
    ExportFvSchenes(openFoamDir);
    ExportFvSolution(openFoamDir);
    // export ROOT geometry to OpenFOAM format
    ExportGeomToFoam(openFoamDir);

    std::cout << "OpenFoam files created" << std::endl;
  } // end ExportFoamFiles


}  // namespace bdm

#endif // OPEN_FOAM_EXPORT_H_
