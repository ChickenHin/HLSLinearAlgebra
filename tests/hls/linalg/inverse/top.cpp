#include "linalg/linalg.h"

extern "C" void top(const double in_a[9], double out[9]) {
    // Convert input array to Mat3
    linalg::Mat3<double> A;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            A(r, c) = in_a[r * 3 + c];
        }
    }
    
    // Compute inverse
    linalg::Mat3<double> Ainv = A.inverse();
    
    // Convert result back to array
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            out[r * 3 + c] = Ainv(r, c);
        }
    }
}
