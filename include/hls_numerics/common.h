#pragma once

#include "hls_math.h"
#include "ap_int.h"
#include "ap_fixed.h"

template <int nbits>
int count_leading_simbol(ap_uint<nbits> bits, bool symbol)
{
#pragma HLS INLINE
    int count = 0;
count_leading_simbol_loop:
    for (int i = nbits - 1; i >= 0; --i)
    {
#pragma HLS UNROLL
        if (bits[i] == symbol)
            count++;
        else
            break;
    }
    return count;
}

template <int nbits, int es>
int count_leading_simbol(ap_fixed<nbits, es> bits, bool symbol)
{
#pragma HLS INLINE
    int count = 0;
count_leading_simbol_loop:
    for (int i = nbits - 1; i >= 0; --i)
    {
#pragma HLS UNROLL
        if (bits[i] == symbol)
            count++;
        else
            break;
    }
    return count;
}
