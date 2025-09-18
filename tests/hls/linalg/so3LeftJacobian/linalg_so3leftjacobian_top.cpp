#include "linalg/linalg.h"

extern "C" void linalg_so3leftjacobian_top(const double phi[3], double J[9]) {
    // Convert input array to Vec3
    linalg::Vec3<double> phi_vec(phi[0], phi[1], phi[2]);
    
    // Compute the left Jacobian
    linalg::Mat3<double> J_mat = linalg::so3LeftJacobian(phi_vec);
    
    // Copy the result to the output array (row-major order)
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            J[r * 3 + c] = J_mat(r, c);
        }
    }
}
