#pragma once

#include "hls_math.h"
#include "ap_int.h"
#include "ap_fixed.h"

template <int nbits>
int count_leading_simbol(ap_uint<nbits> bits, bool symbol)
{
    // #pragma HLS INLINE
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
int count_leading_simbol(ap_ufixed<nbits, es> bits)
{
    // #pragma HLS INLINE
    int count = 0;
count_leading_simbol_loop:
    for (int i = nbits - 1; i >= 0; --i)
    {
#pragma HLS UNROLL
        if (bits[i] == 0)
            count++;
        else
            break;
    }
    return count;
}

template <int nbits, int es>
int count_leading_simbol(ap_fixed<nbits, es> bits, bool symbol)
{
    // #pragma HLS INLINE
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

template <int fbits, int ibits>
ap_ufixed<fbits, ibits> round_to(const ap_ufixed<fbits, ibits> &val, int frac_bit)
{
    // #pragma HLS INLINE

    int first_frac_bit = fbits - 1 - ibits;

    ap_ufixed<fbits, ibits> rval = val;
    if (val[first_frac_bit - frac_bit] == 1)
    {
        ap_ufixed<fbits, ibits> one = 0;
        one[first_frac_bit - frac_bit] = 1;
        // if(pmantissa[0] == 0)
        rval += one;
        // pmantissa[0] = 1;
    }
    return rval;
}
