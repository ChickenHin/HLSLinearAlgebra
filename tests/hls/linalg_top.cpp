#include "linalg/linalg.h"

// Simple, generic top to exercise multiple linalg ops using doubles for interface
// Inputs:
//  - id selects operation
//      0: Mat3 determinant. in_a[0..8] = row-major 3x3. out[0] = det
//      1: Mat3 inverse.     in_a[0..8] = row-major 3x3. out[0..8] = row-major inv
//      2: so3Exp.           in_a[0..2] = phi (3).      out[0..8] = row-major R
//      3: so3LeftJacobian.  in_a[0..2] = phi (3).      out[0..8] = row-major J
//      4: se3Exp.           in_a[0..5] = xi (rho,phi). out[0..15] = row-major 4x4 T
//      5: quat * vec.       in_a[0..3] = q (w,x,y,z), in_b[0..2] = v. out[0..2] = v_out
//  - in_b optionally carries extra inputs for some ops
//  - out carries results (sized generously to 16 doubles)
extern "C" void linalg_top(int id,
                           const double in_a[16],
                           const double in_b[16],
                           double out[16]) {
#pragma HLS INTERFACE s_axilite port=id bundle=control
#pragma HLS INTERFACE s_axilite port=in_a bundle=control
#pragma HLS INTERFACE s_axilite port=in_b bundle=control
#pragma HLS INTERFACE s_axilite port=out bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

    // Zero outputs by default
    for (int i = 0; i < 16; ++i) {
#pragma HLS UNROLL
        out[i] = 0.0;
    }

    if (id == 0) {
        // determinant
        linalg::Mat3<double> M(in_a);
        out[0] = (double)M.determinant();
    } else if (id == 1) {
        // inverse
        linalg::Mat3<double> M(in_a);
        linalg::Mat3<double> Minv = M.inverse();
        // Write row-major
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                out[r * 3 + c] = Minv(r, c);
            }
        }
    } else if (id == 2) {
        // so3Exp
        linalg::Vec3<double> phi(in_a[0], in_a[1], in_a[2]);
        linalg::SO3<double> R = linalg::so3Exp(phi);
        linalg::Mat3<double> Rm = R.matrix();
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                out[r * 3 + c] = Rm(r, c);
            }
        }
    } else if (id == 3) {
        // so3LeftJacobian
        linalg::Vec3<double> phi(in_a[0], in_a[1], in_a[2]);
        linalg::Mat3<double> J = linalg::so3LeftJacobian(phi);
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                out[r * 3 + c] = J(r, c);
            }
        }
    } else if (id == 4) {
        // se3Exp
        linalg::Vec6<double> xi(in_a[0], in_a[1], in_a[2], in_a[3], in_a[4], in_a[5]);
        linalg::SE3<double> T = linalg::se3Exp(xi);
        linalg::Mat4<double> Tm = T.matrix();
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                out[r * 4 + c] = Tm(r, c);
            }
        }
    } else if (id == 5) {
        // quaternion rotate vector
        linalg::Quaternion<double> q(in_a[0], in_a[1], in_a[2], in_a[3]);
        linalg::Vec3<double> v(in_b[0], in_b[1], in_b[2]);
        linalg::Vec3<double> vr = q * v;
        out[0] = vr(0);
        out[1] = vr(1);
        out[2] = vr(2);
    }
}
