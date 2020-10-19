// -----------------------------------------------------------------------------
//
// Copyright (C) Jean de Montigny.
// All Rights Reserved.
//
// -----------------------------------------------------------------------------

#ifndef UTILS_METHODS_
#define UTILS_METHODS_

namespace bdm {

// ------------------------------------------------------------------------
  inline float TriangleArea(double x1, double y1, double x2, double y2, double x3, double y3) {
     return std::abs((x1*(y2-y3) + x2*(y3-y1)+ x3*(y1-y2))/2.0);
  } // end TriangleArea

// ------------------------------------------------------------------------
  inline bool IsInsideTriangle(Double3 tri_a, Double3 tri_b, Double3 tri_c, Double3 point) {
    float a = TriangleArea(tri_a[0], tri_a[1], tri_b[0], tri_b[1], tri_c[0], tri_c[1]);
    float a1 = TriangleArea(point[0], point[1], tri_b[0], tri_b[1], tri_c[0], tri_c[1]);
    float a2 = TriangleArea(tri_a[0], tri_a[1], point[0], point[1], tri_c[0], tri_c[1]);
    float a3 = TriangleArea(tri_a[0], tri_a[1], tri_b[0], tri_b[1], point[0], point[1]);
    return (a == a1 + a2 + a3);
  } // end IsInsideTriangle

// ------------------------------------------------------------------------
  inline double DegToRad (double degrees) {
      return degrees * 4.0 * atan (1.0) / 180.0;
  }

// ------------------------------------------------------------------------
  inline std::vector<Double3> GetDiffusionPositions(Double3 position, double radius, double v[2], int grid_spacing, double d_len, double d_wid) {
    // coordinates of triangle where substance is produced
    // mouth
    Double3 tri_a =
      { position[0] + v[0] * radius,
        position[1] + v[1] * radius, position[2]};
    // further point in agent's orientation, at d_len dist
    Double3 tri_d =
      { tri_a[0] + v[0] * d_len,
        tri_a[1] + v[1] * d_len, position[2]};
    // triangle base coordinates
    // B = 90° of v fom D * d_wid/2
    double theta = DegToRad(90);
    double cs = std::cos(theta);
    double sn = std::sin(theta);
    double w_b[2] = {v[0] * cs - v[1] * sn, v[0] * sn + v[1] * cs};
    Double3 tri_b =
      { tri_d[0] + w_b[0] * d_wid/2,
        tri_d[1] + w_b[1] * d_wid/2,
        position[2]};
    // C = -90° of v fom D * d_wid/2
    theta = DegToRad(-90);
    cs = std::cos(theta);
    sn = std::sin(theta);
    double w_c[2] = {v[0] * cs - v[1] * sn, v[0] * sn + v[1] * cs};
    Double3 tri_c =
    { tri_d[0] + w_c[0] * d_wid/2,
      tri_d[1] + w_c[1] * d_wid/2,
      position[2]};

    std::vector<Double3> diffusion_positions;
    // from A to D with grid_spacing step
    for (int i = 0; i < d_len/grid_spacing; i++) {
      // from B to C with grid_spacing step
      for (int j = 0; j < d_wid/grid_spacing; j++) {
        // potential diffusion point
        double diff_x = tri_b[0] - (v[0] * i * grid_spacing) - (w_b[0] * j * grid_spacing);
        double diff_y = tri_b[1] - (v[1] * i * grid_spacing) - (w_b[1] * j * grid_spacing);
        double diff_z = position[2];
        Double3 point_pos = {diff_x, diff_y, diff_z};
        //if i,j is within cone
        if (IsInsideTriangle(tri_a, tri_b, tri_c, point_pos)) {
          diffusion_positions.push_back(point_pos);
        }
      } // end for j
    } // end for i

    return diffusion_positions;
  } // end GetDiffusionPositions

}

#endif // UTILS_METHODS_
