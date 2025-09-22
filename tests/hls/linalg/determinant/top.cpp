#include "linalg/linalg.h"

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
