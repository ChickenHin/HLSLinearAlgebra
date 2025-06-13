#pragma once

#include "hls_math.h"
#include "ap_int.h"   
#include "ap_fixed.h" 

template <int nbits>
int countLeadingSimbol(ap_uint<nbits> bits, bool symbol)
{
    // #pragma HLS INLINE
    int count = 0;
    for (int i = nbits - 1; i >= 0; --i)
    {
        if (bits[i] == symbol)
            count++;
        else
            break;
    }
    return count;
}