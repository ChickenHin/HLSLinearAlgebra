#include "linalg/linalg.h"

extern "C" void top(const double in_a[9], double out[9])
{

#pragma HLS INTERFACE s_axilite port = in_a bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

#pragma HLS PIPELINE

    linalg::Mat3<double> A(in_a);

    linalg::Mat3<double> Ainv = A.inverse();

    // Convert result back to array
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            out[r * 3 + c] = Ainv(r, c);
        }
    }
}
