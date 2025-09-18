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
extern "C" void top(const double in_a[16],
                    double &out)
{
#pragma HLS INTERFACE s_axilite port = in_a bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    // determinant
    linalg::Mat3<double> M(in_a);
    out = (double)M.determinant();
}
