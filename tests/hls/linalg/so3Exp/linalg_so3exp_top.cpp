#include "linalg/linalg.h"

extern "C" void linalg_so3exp_top(const double phi[3], double R[9])
{
#pragma HLS INTERFACE s_axilite port = phi bundle = control
#pragma HLS INTERFACE s_axilite port = R bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

#pragma HLS PIPELINE

    // Convert input array to Vec3
    linalg::Vec3<double> phi_vec(phi[0], phi[1], phi[2]);

    // Compute the exponential map
    linalg::SO3<double> R_so3 = linalg::so3Exp(phi_vec);

    // Convert the result to a 3x3 matrix
    linalg::Mat3<double> R_mat = R_so3.matrix();

    // Copy the result to the output array (row-major order)
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            R[r * 3 + c] = R_mat(r, c);
        }
    }
}
