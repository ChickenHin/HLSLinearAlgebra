#include "linalg/linalg.h"
#include "hls_numerics/FloatX.h"
#include "hls_numerics/Posit.h"

extern "C" void top(double in_0,
                    double in_1,
                    double in_2,
                    double in_3,
                    double in_4,
                    double in_5,
                    double in_6,
                    double in_7,
                    double in_8,
                    double &out)
{
#pragma HLS INTERFACE s_axilite port = in_0 bundle = control
#pragma HLS INTERFACE s_axilite port = in_1 bundle = control
#pragma HLS INTERFACE s_axilite port = in_2 bundle = control
#pragma HLS INTERFACE s_axilite port = in_3 bundle = control
#pragma HLS INTERFACE s_axilite port = in_4 bundle = control
#pragma HLS INTERFACE s_axilite port = in_5 bundle = control
#pragma HLS INTERFACE s_axilite port = in_6 bundle = control
#pragma HLS INTERFACE s_axilite port = in_7 bundle = control
#pragma HLS INTERFACE s_axilite port = in_8 bundle = control

#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    // #pragma HLS PIPELINE II = 1

    // linalg::Mat3<float> M(in_a);

    linalg::Mat3<float> M;
    M(0, 0) = in_0;
    M(0, 1) = in_1;
    M(0, 2) = in_2;
    M(1, 0) = in_3;
    M(1, 1) = in_4;
    M(1, 2) = in_5;
    M(2, 0) = in_6;
    M(2, 1) = in_7;
    M(2, 2) = in_8;

    out = (double)M.determinant();
}
