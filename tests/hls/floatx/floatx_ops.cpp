#include "hls_numerics/FloatX.h"

// Define the floatx type configuration
using FloatX_32_8 = FloatX<32, 8>;

// Addition operation
extern "C" void floatx_add(double a, double b, double &result) {
#pragma HLS INTERFACE s_axilite port=return bundle=control
#pragma HLS INTERFACE s_axilite port=a bundle=control
#pragma HLS INTERFACE s_axilite port=b bundle=control
#pragma HLS INTERFACE s_axilite port=result bundle=control
    FloatX_32_8 fa(a);
    FloatX_32_8 fb(b);
    FloatX_32_8 fres = fa + fb;
    result = static_cast<double>(fres);
}

// Subtraction operation
extern "C" void floatx_sub(double a, double b, double &result) {
#pragma HLS INTERFACE s_axilite port=return bundle=control
#pragma HLS INTERFACE s_axilite port=a bundle=control
#pragma HLS INTERFACE s_axilite port=b bundle=control
#pragma HLS INTERFACE s_axilite port=result bundle=control
    FloatX_32_8 fa(a);
    FloatX_32_8 fb(b);
    FloatX_32_8 fres = fa - fb;
    result = static_cast<double>(fres);
}

// Multiplication operation
extern "C" void floatx_mul(double a, double b, double &result) {
#pragma HLS INTERFACE s_axilite port=return bundle=control
#pragma HLS INTERFACE s_axilite port=a bundle=control
#pragma HLS INTERFACE s_axilite port=b bundle=control
#pragma HLS INTERFACE s_axilite port=result bundle=control
    FloatX_32_8 fa(a);
    FloatX_32_8 fb(b);
    FloatX_32_8 fres = fa * fb;
    result = static_cast<double>(fres);
}

// Division operation
extern "C" void floatx_div(double a, double b, double &result) {
#pragma HLS INTERFACE s_axilite port=return bundle=control
#pragma HLS INTERFACE s_axilite port=a bundle=control
#pragma HLS INTERFACE s_axilite port=b bundle=control
#pragma HLS INTERFACE s_axilite port=result bundle=control
    FloatX_32_8 fa(a);
    FloatX_32_8 fb(b);
    FloatX_32_8 fres = fa / fb;
    result = static_cast<double>(fres);
}
