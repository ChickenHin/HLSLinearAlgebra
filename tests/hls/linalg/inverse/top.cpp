#include "linalg/linalg.h"
#include "hls_numerics/FloatX.h"
#include "hls_numerics/Posit.h"

extern "C" void top(const double in_a[9], double out[9])
{

#pragma HLS INTERFACE s_axilite port = in_a bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    // #pragma HLS PIPELINE
    //  #pragma HLS DATAFLOW

    linalg::Mat3<Posit<32, 3>> A(in_a);

    linalg::Mat3<Posit<32, 3>> Ainv = A.inverse();

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
}
