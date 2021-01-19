
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
  struct Facet {
    std::vector<float> orientation;
    std::vector<std::vector<float>> vertices;
  };

// ---------------------------------------------------------------------------
  inline std::vector<Facet> GetGeomFacets() {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* param = sim->GetParam();
    auto* sparam = param->GetModuleParam<SimParam>();
    auto* sim_space = gGeoManager->GetVolume("sim_space");

    std::vector<Facet> facets;

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

      // add the 12 triangles vertex coordinates and orientation
      for (int tri = 0; tri < 12; tri ++) {
         Facet f;
         f.orientation = {
           (float)face_orient_list[tri][0],
           (float)face_orient_list[tri][1],
           (float)face_orient_list[tri][2]
         };
         // x/100: cm to m
         f.vertices = {
           {(float)(vert_master[vert_tri_list[tri][0]][0]/100),
             (float)(vert_master[vert_tri_list[tri][0]][1]/100),
             (float)(vert_master[vert_tri_list[tri][0]][2]/100)},
           {(float)(vert_master[vert_tri_list[tri][1]][0]/100),
             (float)(vert_master[vert_tri_list[tri][1]][1]/100), (float)(vert_master[vert_tri_list[tri][1]][2]/100)},
           {(float)(vert_master[vert_tri_list[tri][2]][0]/100),
             (float)(vert_master[vert_tri_list[tri][2]][1]/100),
             (float)(vert_master[vert_tri_list[tri][2]][2]/100)}
         };

         facets.push_back(f);
      }
    } // end for node in sim_space

    // -------- Agents -------- //
    std::vector<Double3> agents_position;
    std::vector<int> agents_state;
    // get humans info
    auto get_agents_lists = [&agents_position, &agents_state](SimObject* so) {
      auto* hu = bdm_static_cast<Human*>(so);
      agents_position.push_back(hu->GetPosition());
      agents_state.push_back(hu->state_);
    };
    rm->ApplyOnAllElements(get_agents_lists);

    for (size_t agent = 0; agent < agents_position.size(); agent++ ) {
      Double3 pos = agents_position[agent];
      // cm to m
      pos[1] = pos[1]/100; pos[2] = pos[2]/100; pos[3] = pos[3]/100;
      // diameter/200: radius from cm to m
      double radius = sparam->human_diameter/200;

      // per geom object: 40 faces (60 vertices)
      Double3 face_orient_list[40] = {
        {0.927957, -0.301511, -0.219061}, {0.506732, -0.697457, -0.506732},
        {0.506732, -0.697457, -0.506732}, {-0.000000, -0.809017, -0.587785},
        {0, -0.809017, -0.587785}, {-0.506732, -0.697457, -0.506732},
        {-0.506732, -0.697457, -0.506732}, {-0.927957, -0.301511, -0.219061},
        {0.927957, 0.115167, -0.354448}, {0.506732, 0.266405, -0.819909},
        {0.506732, 0.266405, -0.819909}, {0, 0.309017, -0.951057},
        {0, 0.309017, -0.951057}, {-0.506732, 0.266405, -0.819909},
        {-0.506732, 0.266405, -0.819909}, {-0.927957, 0.115167, -0.354448},
        {0.927957, 0.372689, -0}, {0.506732, 0.862104, -0},
        {0.506732, 0.862104, -0}, {0, 1.0, -0},
        {0, 1.0, 0}, {-0.506732, 0.862104, 0},
        {-0.506732, 0.862104, 0}, {-0.927957, 0.372689, 0},
        {0.927957, 0.115167, 0.354448}, {0.506732, 0.266405, 0.819909},
        {0.506732, 0.266405, 0.819909}, {0, 0.309017, 0.951057},
        {0, 0.309017, 0.951057}, {-0.506732, 0.266405, 0.819909},
        {-0.506732, 0.266405, 0.819909}, {-0.927957, 0.115167, 0.354448},
        {0.927957, -0.301511, 0.219061}, {0.506732, -0.697457, 0.506732},
        {0.506732, -0.697457, 0.506732}, {0, -0.809017, 0.587785},
        {0, -0.809017, 0.587785}, {-0.506732, -0.697457, 0.506732},
        {-0.506732, -0.697457, 0.506732}, {-0.927957, -0.301511, 0.219061} };
      // vertices positions for a sphere of radius 1 at position 0, 0, 0
      Double3 vert_norm_pos_list[40][3] = {
        {{0.809017, -0.587785, -0}, {0.809017, -0.181636, -0.559017}, {1.0, -0, -0}},
        {{0.809017, -0.587785, -0}, {0.309017, -0.951057, -0}, {0.809017, -0.181636, -0.559017}},
        {{0.309017, -0.951057, -0}, {0.309017, -0.293893, -0.904508}, {0.809017, -0.181636, -0.559017}},
        {{0.309017, -0.951057, -0}, {-0.309017, -0.951057, -0}, {0.309017, -0.293893, -0.904508}},
        {{-0.309017, -0.951057, -0}, {-0.309017, -0.293893, -0.904508}, {0.309017, -0.293893, -0.904508}},
        {{-0.309017, -0.951057, -0}, {-0.809017, -0.587785, -0}, {-0.309017, -0.293893, -0.904508}},
        {{-0.809017, -0.587785, -0}, {-0.809017, -0.181636, -0.559017}, {-0.309017, -0.293893, -0.904508}},
        {{-0.809017, -0.587785, -0}, {-1.0, -0, -0}, {-0.809017, -0.181636, -0.559017}},
        {{0.809017, -0.181636, -0.559017}, {0.809017, 0.475528, -0.345491}, {1.0, -0, -0}},
        {{0.809017, -0.181636, -0.559017}, {0.309017, -0.293893, -0.904508}, {0.809017, 0.475528, -0.345491}},
        {{0.309017, -0.293893, -0.904508}, {0.309017, 0.769421, -0.559017}, {0.809017, 0.475528, -0.345491}},
        {{0.309017, -0.293893, -0.904508}, {-0.309017, -0.293893, -0.904508}, {0.309017, 0.769421, -0.559017}},
        {{-0.309017, -0.293893, -0.904508}, {-0.309017, 0.769421, -0.559017}, {0.309017, 0.769421, -0.559017}},
        {{-0.309017, -0.293893, -0.904508}, {-0.809017, -0.181636, -0.559017}, {-0.309017, 0.769421, -0.559017}},
        {{-0.809017, -0.181636, -0.559017}, {-0.809017, 0.475528, -0.345491}, {-0.309017, 0.769421, -0.559017}},
        {{-0.809017, -0.181636, -0.559017}, {-1.0, -0, -0}, {-0.809017, 0.475528, -0.345491}},
        {{0.809017, 0.475528, -0.345491}, {0.809017, 0.475528, 0.345491}, {1.0, -0, -0}},
        {{0.809017, 0.475528, -0.345491}, {0.309017, 0.769421, -0.559017}, {0.809017, 0.475528, 0.345491}},
        {{0.309017, 0.769421, -0.559017}, {0.309017, 0.769421, 0.559017}, {0.809017, 0.475528, 0.345491}},
        {{0.309017, 0.769421, -0.559017}, {-0.309017, 0.769421, -0.559017}, {0.309017, 0.769421, 0.559017}},
        {{-0.309017, 0.769421, -0.559017}, {-0.309017, 0.769421, 0.559017}, {0.309017, 0.769421, 0.559017}},
        {{-0.309017, 0.769421, -0.559017}, {-0.809017, 0.475528, -0.345491}, {-0.309017, 0.769421, 0.559017}},
        {{-0.809017, 0.475528, -0.345491}, {-0.809017, 0.475528, 0.345491}, {-0.309017, 0.769421, 0.559017}},
        {{-0.809017, 0.475528, -0.345491}, {-1.0, -0, -0}, {-0.809017, 0.475528, 0.345491}},
        {{0.809017, 0.475528, 0.345491}, {0.809017, -0.181636, 0.559017}, {1.0, -0, -0}},
        {{0.809017, 0.475528, 0.345491}, {0.309017, 0.769421, 0.559017}, {0.809017, -0.181636, 0.559017}},
        {{0.309017, 0.769421, 0.559017}, {0.309017, -0.293893, 0.904508}, {0.809017, -0.181636, 0.559017}},
        {{0.309017, 0.769421, 0.559017}, {-0.309017, 0.769421, 0.559017}, {0.309017, -0.293893, 0.904508}},
        {{-0.309017, 0.769421, 0.559017}, {-0.309017, -0.293893, 0.904508}, {0.309017, -0.293893, 0.904508}},
        {{-0.309017, 0.769421, 0.559017}, {-0.809017, 0.475528, 0.345491}, {-0.309017, -0.293893, 0.904508}},
        {{-0.809017, 0.475528, 0.345491}, {-0.809017, -0.181636, 0.559017}, {-0.309017, -0.293893, 0.904508}},
        {{-0.809017, 0.475528, 0.345491}, {-1.0, -0, -0}, {-0.809017, -0.181636, 0.559017}},
        {{0.809017, -0.181636, 0.559017}, {0.809017, -0.587785, -0}, {1.0, -0, -0}},
        {{0.809017, -0.181636, 0.559017}, {0.309017, -0.293893, 0.904508}, {0.809017, -0.587785, -0}},
        {{0.309017, -0.293893, 0.904508}, {0.309017, -0.951057, -0}, {0.809017, -0.587785, -0}},
        {{0.309017, -0.293893, 0.904508}, {-0.309017, -0.293893, 0.904508}, {0.309017, -0.951057, -0}},
        {{-0.309017, -0.293893, 0.904508}, {-0.309017, -0.951057, -0}, {0.309017, -0.951057, -0}},
        {{-0.309017, -0.293893, 0.904508}, {-0.809017, -0.181636, 0.559017}, {-0.309017, -0.951057, -0}},
        {{-0.809017, -0.181636, 0.559017}, {-0.809017, -0.587785, -0}, {-0.309017, -0.951057, -0}},
        {{-0.809017, -0.181636, 0.559017}, {-1.0, -0, -0}, {-0.809017, -0.587785, -0}} };

      for (int tri = 0; tri < 40; tri ++) {
        Facet f;
        f.orientation = {
          (float)face_orient_list[tri][0],
          (float)face_orient_list[tri][1],
          (float)face_orient_list[tri][2]
        };
        float vert_ax = (float)pos[0]+vert_norm_pos_list[tri][0][0]*radius;
        float vert_ay = (float)pos[1]+vert_norm_pos_list[tri][0][1]*radius;
        float vert_az = (float)pos[2]+vert_norm_pos_list[tri][0][2]*radius;
        float vert_bx = (float)pos[0]+vert_norm_pos_list[tri][1][0]*radius;
        float vert_by = (float)pos[1]+vert_norm_pos_list[tri][1][1]*radius;
        float vert_bz = (float)pos[2]+vert_norm_pos_list[tri][1][2]*radius;
        float vert_cx = (float)pos[0]+vert_norm_pos_list[tri][2][0]*radius;
        float vert_cy = (float)pos[1]+vert_norm_pos_list[tri][2][1]*radius;
        float vert_cz = (float)pos[2]+vert_norm_pos_list[tri][2][2]*radius;
        f.vertices = {
          {vert_ax, vert_ay, vert_az},
          {vert_bx, vert_by, vert_bz},
          {vert_cx, vert_cy, vert_cz}
        };

        facets.push_back(f);
      }
    } // end for each agent in sim

    return facets;

 } // end GetGeomTriangles

// ---------------------------------------------------------------------------
  inline void ExportGeomToAst(std::string openlbDir) {
    // ascii StL format (ast)
    std::vector<Facet> facets = GetGeomFacets();

    // create file for export
    std::ofstream ast_file;
    ast_file.open(Concat(openlbDir, "bus.ast"));

    ast_file << "solid Mesh";

    for (size_t i = 0; i < facets.size(); i++){
      ast_file << "  facet normal "
               << facets[i].orientation[0] << " "
               << facets[i].orientation[1] << " "
               << facets[i].orientation[2] << "\n"
               << "    outer loop\n"
               << "      vertex " << facets[i].vertices[0][0]
               << " " << facets[i].vertices[0][1]
               << " " << facets[i].vertices[0][2] << "\n"
               << "      vertex " << facets[i].vertices[1][0]
               << " " << facets[i].vertices[1][1]
               << " " << facets[i].vertices[1][2] << "\n"
               << "      vertex " << facets[i].vertices[2][0]
               << " " << facets[i].vertices[2][1]
               << " " << facets[i].vertices[2][2] << "\n"
               << "    endloop\n"
               << "  endfacet\n";
    }

    ast_file << "endsolid Mesh";
    ast_file.close();

    std::cout << "ascii StL file (ast) created" << std::endl;
  } // end ExportGeomToAst
// ---------------------------------------------------------------------------
  inline void ExportGeomToBinaryStl(std::string openlbDir) {
    // binary StL format
    std::vector<Facet> facets = GetGeomFacets();

    // create binary file for export
    std::ofstream stl_file;
    stl_file.open(Concat(openlbDir, "bus.stl"),
      std::ios::out | std::ios::binary);

    std::string header = "bus Mesh";
    char head[80];
    std::strncpy(head, header.c_str(), sizeof(head)-1);
    char attribute[2] = "0";
    unsigned long n_triangles = facets.size();

    stl_file.write(head, 80);
    stl_file.write((char*)&n_triangles, 4);

    for (size_t i = 0; i < facets.size(); i++){
      // orientation
      stl_file.write((char*)&facets[i].orientation[0], 4);
      stl_file.write((char*)&facets[i].orientation[1], 4);
      stl_file.write((char*)&facets[i].orientation[2], 4);
      // first vertice
      stl_file.write((char*)&facets[i].vertices[0][0], 4);
      stl_file.write((char*)&facets[i].vertices[0][1], 4);
      stl_file.write((char*)&facets[i].vertices[0][2], 4);
      // second vertice
      stl_file.write((char*)&facets[i].vertices[1][0], 4);
      stl_file.write((char*)&facets[i].vertices[1][1], 4);
      stl_file.write((char*)&facets[i].vertices[1][2], 4);
      // third vertice
      stl_file.write((char*)&facets[i].vertices[2][0], 4);
      stl_file.write((char*)&facets[i].vertices[2][1], 4);
      stl_file.write((char*)&facets[i].vertices[2][2], 4);
      // attribute (empty)
      stl_file.write(attribute, sizeof(attribute));
    }

    stl_file.close();

    std::cout << "Binary StL file created" << std::endl;
  } // end ExportGeomToBinaryStl

// ---------------------------------------------------------------------------
  inline void ExportOpenlbFiles(std::string openlbDir, std::string format) {
    // create openLB directory
    if (!std::system(Concat("mkdir ", openlbDir).c_str())) {
      std::cout << "openLB directory created" << std::endl;
    }

    // export ROOT geometry to ascii StL format (.ast)
    if (format == "ast") {
      ExportGeomToAst(openlbDir);
    }
    // export ROOT geometry to binary StL format (.stl)
    else if (format == "stl") {
      ExportGeomToBinaryStl(openlbDir);
    }
    else {
      std::cout << "Wrong argument for stl file. Please choose [ast] or [stl]"
                << std::endl;
    }
  } // end ExportOpenlbFiles


}  // namespace bdm

#endif // OPENLB_EXPORT_H_
