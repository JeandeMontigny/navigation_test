
#ifndef OPEN_FOAM_EXPORT_H_
#define OPEN_FOAM_EXPORT_H_

#include "TGeometry.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "core/container/math_array.h"
#include "sim-param.h"
#include "biodynamo.h"

namespace bdm {

// ---------------------------------------------------------------------------
  inline void ExportGeomToFoam(std::string openFoamDir) {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();
    double min_b = param->min_bound_;
    double max_b = param->max_bound_;

    auto* sim_space = gGeoManager->GetVolume("sim_space");

    // create file for export
    std::ofstream geometry_file;
    geometry_file.open(Concat(openFoamDir, "system/blockMeshDict"));

      std::string head;
      head += Concat("FoamFile\n{\n"
        , "\tversion\t2.0;\n\tformat\tascii;\n"
        , "\tclass\tdictionary;\n\tobject\tblockMeshDict;\n"
        , "}\n"
        , "\nconvertToMeters 0.01;\n");

      std::string vertices;
      // TODO: get min and max x, y, z coord of simu
      vertices += Concat("\nvertices\n(\n"
      , "\t(", -550, " " , -126 , " ", -126, ")\n"
      , "\t(", 550, " " , -126 , " ", -126, ")\n"
      , "\t(", 550, " " , 126 , " ", -126, ")\n"
      , "\t(", -550, " " , 126 , " ", -126, ")\n"
      , "\t(", -550, " " , -126 , " ", 126, ")\n"
      , "\t(", 550, " " , -126 , " ", 126, ")\n"
      , "\t(", 550, " " , 126 , " ", 126, ")\n"
      , "\t(", -550, " " , 126 , " ", 126, ")\n");

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
        vertices += Concat("\t(", vert_master_sorted[vert_i][0], " "
          , vert_master_sorted[vert_i][1], " "
          , vert_master_sorted[vert_i][2], ")\n");
      } // end write each vertice of this box
      // add each vertex of this box to blocks section
      blocks += Concat("\thex (",i*8+8, " ", i*8+9, " ", i*8+10, " "
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

    boundaries += "\t\t);\n\t}\n"; // end boxes section of boundary

    // last vertice index used for geometry
    int last_geom_vertice = sim_space->GetNdaughters()*8+7;


    std::vector<Double3> agents_position;
    std::vector<std::vector<double>> agents_direction;
    std::vector<int> agents_state;
    // get humans info
    auto get_agents_lists = [&agents_position, &agents_direction, &agents_state](SimObject* so) {
      auto* hu = bdm_static_cast<Human*>(so);
      agents_position.push_back(hu->GetPosition());
      agents_direction.push_back(hu->orientation_);
      agents_state.push_back(hu->state_);
    };
    rm->ApplyOnAllElements(get_agents_lists);

    std::string agents_geometry = "\ngeometry\n{\n";
    std::string agents_vertices;
    std::string agents_blocks;
    std::string agents_edges = "\nedges\n(\n";
    std::string agents_faces = "\nfaces\n(\n";
    std::string agents_boundaries = Concat(
      "\theads\n\t{\n"
      , "\t\ttype wall;\n"
      , "\t\tfaces\n"
      , "\t\t(\n");

    int number_of_inlets = 0;
    std::string inlet_boundaries = Concat(
      "\tinlets\n\t{\n"
      , "\t\ttype patch;\n"
      , "\t\tfaces\n"
      , "\t\t(\n");

    for (size_t agent = 0; agent < agents_position.size(); agent++ ) {
      Double3 pos = agents_position[agent];
      auto dir = agents_direction[agent];
      double radius = sparam->human_diameter/2;

      agents_geometry += Concat("\tsphere\n\t{\n"
        , "\t\ttype\tsearchableSphere;\n"
        , "\t\tcentre\t(", pos[0], " ", pos[1], " ", pos[2], ");\n"
        , "\t\tradius\t", radius, ";\n"
        , "\t}\n");

      double v = 0.5773502 * radius;
      agents_vertices += Concat(
        "\t(", -v+pos[0], " ", -v+pos[1], " ", -v+pos[2], ")\n"
        , "\t(", v+pos[0], " ", -v+pos[1], " ", -v+pos[2], ")\n"
        , "\t(", v+pos[0], " ", v+pos[1], " ", -v+pos[2], ")\n"
        , "\t(", -v+pos[0], " ", v+pos[1], " ", -v+pos[2], ")\n"
        , "\t(", -v+pos[0], " ", -v+pos[1], " ", v+pos[2], ")\n"
        , "\t(", v+pos[0], " ", -v+pos[1], " ", v+pos[2], ")\n"
        , "\t(", v+pos[0], " ", v+pos[1], " ", v+pos[2], ")\n"
        , "\t(", -v+pos[0], " ", v+pos[1], " ", v+pos[2], ")\n");

      agents_blocks += Concat(
        "\thex (",last_geom_vertice+agent*8+number_of_inlets*4+1, " "
        , last_geom_vertice+agent*8+number_of_inlets*4 +2
        , " ", last_geom_vertice+agent*8+number_of_inlets*4+3, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+4
        , " ", last_geom_vertice+agent*8+number_of_inlets*4+5, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+6
        , " ", last_geom_vertice+agent*8+number_of_inlets*4+7, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+8
        , ") (1 1 1) simpleGrading (1 1 1)\n");

      double a = 0.7071067 * radius;
      agents_edges += Concat(
      "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+1, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+2
      , " (", 0+pos[0], " ", -a+pos[1], " ", -a+pos[2], ")\n"
      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+3, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+4
      , " (", 0+pos[0], " ", a+pos[1], " ", -a+pos[2], ")\n"
      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+7, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+8
      , " (", 0+pos[0], " ", a+pos[1], " ", a+pos[2], ")\n"
      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+5, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+6
      , " (", 0+pos[0], " ", -a+pos[1], " ", a+pos[2], ")\n"

      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+1, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+4
      , " (", -a+pos[0], " ", 0+pos[1], " ", -a+pos[2], ")\n"
      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+2, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+3
      , " (", a+pos[0], " ", 0+pos[1], " ", -a+pos[2], ")\n"
      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+6, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+7
      , " (", a+pos[0], " ", 0+pos[1], " ", a+pos[2], ")\n"
      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+5, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+8
      , " (", -a+pos[0], " ", 0+pos[1], " ", a+pos[2], ")\n"

      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+1, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+5
      , " (", -a+pos[0], " ", -a+pos[1], " ", 0+pos[2], ")\n"
      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+2, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+6
      , " (", a+pos[0], " ", -a+pos[1], " ", 0+pos[2], ")\n"
      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+3, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+7
      , " (", a+pos[0], " ", a+pos[1], " ", 0+pos[2], ")\n"
      , "\tarc ", last_geom_vertice+agent*8+number_of_inlets*4+4, " "
      , last_geom_vertice+agent*8+number_of_inlets*4+8
      , " (", -a+pos[0], " ", a+pos[1], " ", 0+pos[2], ")\n");

      agents_faces += Concat(
        "\tproject (", last_geom_vertice+agent*8+number_of_inlets*4+1, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+5, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+8, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+4,") sphere\n"

        , "\tproject (", last_geom_vertice+agent*8+number_of_inlets*4+3, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+7, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+6, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+2,") sphere\n"

        , "\tproject (", last_geom_vertice+agent*8+number_of_inlets*4+2, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+6, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+5, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+1,") sphere\n"

        , "\tproject (", last_geom_vertice+agent*8+number_of_inlets*4+4, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+8, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+7, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+3,") sphere\n"

        , "\tproject (", last_geom_vertice+agent*8+number_of_inlets*4+1, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+4, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+3, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+2,") sphere\n"

        , "\tproject (", last_geom_vertice+agent*8+number_of_inlets*4+5, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+6, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+7, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+8,") sphere\n"
      );

      agents_boundaries += Concat(
        "\t\t\t(", last_geom_vertice+agent*8+number_of_inlets*4+1, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+5, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+8, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+4, ")\n"
        , "\t\t\t(", last_geom_vertice+agent*8+number_of_inlets*4+3, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+7, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+6, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+2, ")\n"
        , "\t\t\t(", last_geom_vertice+agent*8+number_of_inlets*4+2, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+6, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+5, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+1, ")\n"
        , "\t\t\t(", last_geom_vertice+agent*8+number_of_inlets*4+4, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+8, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+7, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+3, ")\n"
        , "\t\t\t(", last_geom_vertice+agent*8+number_of_inlets*4+1, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+4, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+3, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+2, ")\n"
        , "\t\t\t(", last_geom_vertice+agent*8+number_of_inlets*4+5, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+6, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+7, " "
        , last_geom_vertice+agent*8+number_of_inlets*4+8, ")\n");

        // add agent's mouth position and direction for spreading (inlet)
        // TODO:  only for infected? or normal breathing of health population as well?
        if (agents_state[agent] == State::kInfected) {
          Double3 spread_pos = {pos[0] + dir[0]*radius, pos[1] + dir[1]*radius, pos[2]};

          // from spread_pos, has to create a vertical square, perpendicular to the sphere
          // A and B = 90° of dir fom spread_pos, -/+ 1 in z
          double theta = DegToRad(90);
          double cs = std::cos(theta);
          double sn = std::sin(theta);
          double w_p[2] = {dir[0]*cs - dir[1]*sn, dir[0]*sn + dir[1]*cs};
          Double3 spread_verta =
            { spread_pos[0] + w_p[0], spread_pos[1] + w_p[1], spread_pos[2]-1};
          Double3 spread_vertb =
            { spread_pos[0] + w_p[0], spread_pos[1] + w_p[1], spread_pos[2]+1};

          // C and D = -90° of dir fom spread_pos, -/+ 1 in z
          theta = DegToRad(-90);
          cs = std::cos(theta);
          sn = std::sin(theta);
          double w_m[2] = {dir[0]*cs - dir[1]*sn, dir[0]*sn + dir[1]*cs};
          Double3 spread_vertc =
          { spread_pos[0] + w_m[0], spread_pos[1] + w_m[1], spread_pos[2]-1};
          Double3 spread_vertd =
          { spread_pos[0] + w_m[0], spread_pos[1] + w_m[1], spread_pos[2]+1};

          // sort vertices a, b, c, d
          // ---
          // ++-
          // +++
          // --+
          Double3 spread_vert_unsorted[4] = {spread_verta, spread_vertb, spread_vertc, spread_vertd};
          Double3 spread_vert_sorted[4];
          double sp_vert_x_max = min_b; double sp_vert_x_min = max_b;
          double sp_vert_y_max = min_b; double sp_vert_y_min = max_b;
          double sp_vert_z_max = min_b; double sp_vert_z_min = max_b;

          // get min max for x, y and z
          for (int sp_vert_i = 0; sp_vert_i < 4; sp_vert_i++) {
            if (spread_vert_unsorted[sp_vert_i][0] > sp_vert_x_max) {
              sp_vert_x_max = spread_vert_unsorted[sp_vert_i][0];
            }
            if (spread_vert_unsorted[sp_vert_i][0] < sp_vert_x_min) {
              sp_vert_x_min = spread_vert_unsorted[sp_vert_i][0];
            }
            if (spread_vert_unsorted[sp_vert_i][1] > sp_vert_y_max) {
              sp_vert_y_max = spread_vert_unsorted[sp_vert_i][1];
            }
            if (spread_vert_unsorted[sp_vert_i][1] < sp_vert_y_min) {
              sp_vert_y_min = spread_vert_unsorted[sp_vert_i][1];
            }
            if (spread_vert_unsorted[sp_vert_i][2] > sp_vert_z_max) {
              sp_vert_z_max = spread_vert_unsorted[sp_vert_i][2];
            }
            if (spread_vert_unsorted[sp_vert_i][2] < sp_vert_z_min) {
              sp_vert_z_min = spread_vert_unsorted[sp_vert_i][2];
            }
          }
          // sort
          for (int sp_vert_i = 0; sp_vert_i < 4; sp_vert_i++) {
            if (spread_vert_unsorted[sp_vert_i][0] == sp_vert_x_min
              && spread_vert_unsorted[sp_vert_i][1] == sp_vert_y_min
              && spread_vert_unsorted[sp_vert_i][2] == sp_vert_z_min) {
              spread_vert_sorted[0] = spread_vert_unsorted[sp_vert_i];
            }
            if (spread_vert_unsorted[sp_vert_i][0] == sp_vert_x_max
              && spread_vert_unsorted[sp_vert_i][1] == sp_vert_y_min
              && spread_vert_unsorted[sp_vert_i][2] == sp_vert_z_min) {
              spread_vert_sorted[1] = spread_vert_unsorted[sp_vert_i];
            }
            if (spread_vert_unsorted[sp_vert_i][0] == sp_vert_x_max
              && spread_vert_unsorted[sp_vert_i][1] == sp_vert_y_max
              && spread_vert_unsorted[sp_vert_i][2] == sp_vert_z_max) {
              spread_vert_sorted[2] = spread_vert_unsorted[sp_vert_i];
            }
            if (spread_vert_unsorted[sp_vert_i][0] == sp_vert_x_min
              && spread_vert_unsorted[sp_vert_i][1] == sp_vert_y_max
              && spread_vert_unsorted[sp_vert_i][2] == sp_vert_z_max) {
              spread_vert_sorted[3] = spread_vert_unsorted[sp_vert_i];
            }
          }

          agents_vertices += Concat(
            "\t(", spread_vert_sorted[0][0], " ", spread_vert_sorted[0][1]
            , " ", spread_vert_sorted[0][2], ")\n"
            , "\t(", spread_vert_sorted[1][0], " ", spread_vert_sorted[1][1]
            , " ", spread_vert_sorted[1][2], ")\n"
            , "\t(", spread_vert_sorted[2][0], " ", spread_vert_sorted[2][1]
            , " ", spread_vert_sorted[2][2], ")\n"
            , "\t(", spread_vert_sorted[3][0], " ", spread_vert_sorted[3][1]
            , " ", spread_vert_sorted[3][2], ")\n");

            // WARNING: plane instead of cube. may trigger issues later
            //          may have to create cube, with one face as inlate
            agents_blocks += Concat(
              "\thex (",last_geom_vertice+(agent+1)*8+number_of_inlets*4+1, " "
              , last_geom_vertice+(agent+1)*8+number_of_inlets*4 +2, " "
              , last_geom_vertice+(agent+1)*8+number_of_inlets*4+3, " "
              , last_geom_vertice+(agent+1)*8+number_of_inlets*4+4, " "
              , last_geom_vertice+(agent+1)*8+number_of_inlets*4+1, " "
              , last_geom_vertice+(agent+1)*8+number_of_inlets*4 +2 , " "
              , last_geom_vertice+(agent+1)*8+number_of_inlets*4+3, " "
              , last_geom_vertice+(agent+1)*8+number_of_inlets*4+4
              , ") (1 1 1) simpleGrading (1 1 1)\n");

          inlet_boundaries += Concat(
            "\t\t\t(", last_geom_vertice+(agent+1)*8+number_of_inlets*4+1
            , " ",  last_geom_vertice+(agent+1)*8+number_of_inlets*4+2
            , " ",  last_geom_vertice+(agent+1)*8+number_of_inlets*4+3
            , " ",  last_geom_vertice+(agent+1)*8+number_of_inlets*4+4, ")\n");

          number_of_inlets++;
        } // if kInfected

    } // end for each agent in sim

    agents_geometry += "};\n"; // end geometry section
    agents_vertices += ");\n"; // end vertices section
    agents_blocks += ");\n"; // end blocks section
    agents_edges += ");\n"; // end edges section
    agents_faces += ");\n"; // end faces section
    agents_boundaries += "\t\t);\n\t}\n"; // end agents boudaries (not section)

    inlet_boundaries += "\t\t);\n\t}\n);\n"; // end boundary section

    geometry_file << head;
    geometry_file << agents_geometry;
    geometry_file << vertices << agents_vertices;
    geometry_file << blocks << agents_blocks;
    geometry_file << agents_edges;
    geometry_file << agents_faces;
    geometry_file << boundaries << agents_boundaries << inlet_boundaries;
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
