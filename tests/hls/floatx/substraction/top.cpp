#include "hls_numerics/FloatX.h"

// Define the custom float type for synthesis
// Using 32 bits with 8 exponent bits, similar to standard float
constexpr int N_BITS = 32;
constexpr int E_BITS = 8;
using Fx = FloatX<N_BITS, E_BITS>;

extern "C" void top(double in_a, double in_b, double &out)
{
// HLS pragmas to define the interface for the hardware kernel
#pragma HLS INTERFACE s_axilite port = in_a bundle = control
#pragma HLS INTERFACE s_axilite port = in_b bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    // Convert inputs from double to FloatX
    Fx fx_a(in_a);
    Fx fx_b(in_b);

    // Perform arithmetic operations and convert back to double for output
    out = (double)(fx_a - fx_b);
}