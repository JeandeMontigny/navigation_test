
#ifndef OPENLB_EXPORT_H_
#define OPENLB_EXPORT_H_

#include "TGeometry.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "core/container/math_array.h"
#include "sim-param.h"
#include "biodynamo.h"

namespace bdm {

// ---------------------------------------------------------------------------
  inline void ExportGeomToAst(std::string openlbDir) {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();
    // double min_b = param->min_bound_;
    // double max_b = param->max_bound_;

    auto* sim_space = gGeoManager->GetVolume("sim_space");

    // create file for export
    std::ofstream ast_file;
    ast_file.open(Concat(openlbDir, "bus.ast"));

    ast_file << "solid Mesh";

    // -------- ROOT Geometry -------- //
    double vert[24] = {0};
    double vert_master[8][3];
    // for node in sim_space
    for (int i = 0; i < sim_space->GetNdaughters(); i++) {
      auto node = sim_space->GetNode(i);
      TGeoBBox *box = (TGeoBBox*)node->GetVolume()->GetShape();

      // copies the box vertices in local coordinates
      box->SetBoxPoints(&vert[0]);
      node->GetName(); // name of this box
      // for each vertex of this box
      for (auto point = 0; point < 8; point++) {
        // Convert each vertex to the reference frame of sim_space
        node->LocalToMaster(&vert[3*point], vert_master[point]);
      } // end for all vertices points

      // per geom object: 8 vertices forming 12 triangles
      // faces orientation (facing outside of geom object)
      int face_orient_list[12][3] = {
        {0,0,-1}, {0,0,-1}, {0,-1,0}, {0,-1,0}, {-1,0,0}, {-1,0,0},
        {1,0,0}, {1,0,0}, {0,1,0}, {0,1,0}, {0,0,1}, {0,0,1} };
      // each triangle from the 8 vertices
      int vert_tri_list[12][3] = {
        {0,1,2}, {0,2,3}, {4,5,1}, {4,1,0}, {4,0,3}, {4,3,7},
        {3,2,6}, {3,6,7}, {1,5,6}, {1,6,2}, {7,6,5}, {7,5,4} };

      // export the 12 triangles vertex coordinates and orientation
      for (int tri = 0; tri < 12; tri ++) {
        ast_file << "  facet normal "
                 << face_orient_list[tri][0] << " "
                 << face_orient_list[tri][1] << " "
                 << face_orient_list[tri][2] << "\n"
                 << "    outer loop\n"
                 << "      vertex " << vert_master[vert_tri_list[tri][0]][0]
                 << " " << vert_master[vert_tri_list[tri][0]][1]
                 << " " << vert_master[vert_tri_list[tri][0]][2] << "\n"
                 << "      vertex " << vert_master[vert_tri_list[tri][1]][0]
                 << " " << vert_master[vert_tri_list[tri][1]][1]
                 << " " << vert_master[vert_tri_list[tri][1]][2] << "\n"
                 << "      vertex " << vert_master[vert_tri_list[tri][2]][0]
                 << " " << vert_master[vert_tri_list[tri][2]][1]
                 << " " << vert_master[vert_tri_list[tri][2]][2] << "\n"
                 << "    endloop\n"
                 << "  endfacet\n";
      }
    } // end for node in sim_space

    // -------- Agents -------- //
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

    for (size_t agent = 0; agent < agents_position.size(); agent++ ) {
      Double3 pos = agents_position[agent];
      auto dir = agents_direction[agent];
      double radius = sparam->human_diameter/2;

      // per geom object: ?
      // TODO: create shpere with FreeCAD and have a look how it is created as ast file

      ast_file << "  facet normal\n"
               // << orientation << "\n"
               << "    outer loop\n"
               // << "      vertex " << vertex << "\n"
               // << "      vertex " << vertex << "\n"
               // << "      vertex " << vertex << "\n"
               << "    endloop\n"
               << "  endfacet\n";

      // add agent's mouth position and direction for spreading (inlet)
      // TODO:  only for infected? or normal breathing of health population as well?
      if (agents_state[agent] == State::kInfected) {
        Double3 spread_pos = {pos[0] + dir[0]*radius, pos[1] + dir[1]*radius, pos[2]};

        // TODO: spread position as a point is okay for openLB?
        //       otherwise, create a square at spread position +1-1

      } // if kInfected

    } // end for each agent in sim

    ast_file << "endsolid Mesh";

    ast_file.close();
  } // end ExportGeomToAst

// ---------------------------------------------------------------------------
  inline void ExportOpenlbFiles(std::string openlbDir) {
    // create openLB directory
    if (!std::system(Concat("mkdir ", openlbDir).c_str())) {
      std::cout << "openLB directory created" << std::endl;
    }

    // export ROOT geometry to ascii StL format (.ast)
    // TODO: binary StL format
    ExportGeomToAst(openlbDir);

    std::cout << "StL file created" << std::endl;
  } // end ExportOpenlbFiles


}  // namespace bdm

#endif // OPENLB_EXPORT_H_