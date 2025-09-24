#include "hls_numerics/Posit.h"

extern "C" void top(float in_a, float &out)
{
#pragma HLS INTERFACE s_axilite port = in_a bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

//#pragma HLS PIPELINE

    Posit<32, 3> fx_a(in_a);
    out = (float)(fx_a);
}