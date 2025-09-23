#include "hls_numerics/FloatX.h"

extern "C" void top(float in_a, float &out)
{
#pragma HLS INTERFACE s_axilite port = in_a bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

#pragma HLS PIPELINE

    FloatX<32, 8> fx_a(in_a);
    out = (float)(fx_a);
}