#include "linalg/linalg.h"

extern "C" void linalg_se3exp_top(const double xi[6], double T[16]) {
    // Convert input array to Vec6
    linalg::Vec6<double> xi_vec(xi[0], xi[1], xi[2], xi[3], xi[4], xi[5]);
    
    // Compute the exponential map
    linalg::SE3<double> T_se3 = linalg::se3Exp(xi_vec);
    
    // Convert the result to a 4x4 matrix
    linalg::Mat4<double> T_mat = T_se3.matrix();
    
    // Copy the result to the output array (row-major order)
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            T[r * 4 + c] = T_mat(r, c);
        }
    }
}
