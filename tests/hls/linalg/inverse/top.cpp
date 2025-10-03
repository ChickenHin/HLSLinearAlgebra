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
                    double &out_0,
                    double &out_1,
                    double &out_2,
                    double &out_3,
                    double &out_4,
                    double &out_5,
                    double &out_6,
                    double &out_7,
                    double &out_8)
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

#pragma HLS INTERFACE s_axilite port = out_0 bundle = control
#pragma HLS INTERFACE s_axilite port = out_1 bundle = control
#pragma HLS INTERFACE s_axilite port = out_2 bundle = control
#pragma HLS INTERFACE s_axilite port = out_3 bundle = control
#pragma HLS INTERFACE s_axilite port = out_4 bundle = control
#pragma HLS INTERFACE s_axilite port = out_5 bundle = control
#pragma HLS INTERFACE s_axilite port = out_6 bundle = control
#pragma HLS INTERFACE s_axilite port = out_7 bundle = control
#pragma HLS INTERFACE s_axilite port = out_8 bundle = control

#pragma HLS INTERFACE s_axilite port = return bundle = control

#pragma HLS PIPELINE
    //   #pragma HLS DATAFLOW

    // linalg::Mat3<float> A(in_a);

    linalg::Mat3<float> A;

    A(0, 0) = in_0;
    A(0, 1) = in_1;
    A(0, 2) = in_2;
    A(1, 0) = in_3;
    A(1, 1) = in_4;
    A(1, 2) = in_5;
    A(2, 0) = in_6;
    A(2, 1) = in_7;
    A(2, 2) = in_8;

    linalg::Mat3<float> Ainv = A.inverse();

    out_0 = (double)Ainv(0, 0);
    out_1 = (double)Ainv(0, 1);
    out_2 = (double)Ainv(0, 2);
    out_3 = (double)Ainv(1, 0);
    out_4 = (double)Ainv(1, 1);
    out_5 = (double)Ainv(1, 2);
    out_6 = (double)Ainv(2, 0);
    out_7 = (double)Ainv(2, 1);
    out_8 = (double)Ainv(2, 2);
    /*
    // Convert result back to array
    copy_mat_loop_1:
        for (int r = 0; r < 3; ++r)
        {
        copy_map_loop_2:
            for (int c = 0; c < 3; ++c)
            {
    #pragma HLS UNROLL
                out[r * 3 + c] = (double)Ainv(r, c);
            }
        }
            */
}
