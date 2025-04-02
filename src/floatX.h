#pragma once

#include "hls_math.h"
#include "ap_int.h"
#include "ap_fixed.h"

template <typename type>
int count_leading_zeros(type mantissa)
{
    #pragma HLS INLINE

    int leading_zeros = 0;
    count_leading_zeros_loop:
    for(int i = mantissa.width-1; i >= 0; i--)
    {
        if(mantissa[i] == 0)
            leading_zeros++;
        else
            break;
    }
    return leading_zeros;
}

template <int mantissa_size, int exponent_size>
struct floatX
{
    floatX()
    {
        
    }
    /*
    floatX(float c)
    {
        #pragma HLS INLINE

        float fp = c;
        
        float abs_fp = fabs(fp);

        if(abs_fp == 0.0)
        {
            sign = 0;
            mantissa = 0;
            exponent = 0;
        }
        else
        {
            bool s = 0;
            if(fp < 0.0)
                s = 1;

            float exp = floor(hls::log2(abs_fp));
            float quo = hls::pow(2.0f, exp);
            float man = abs_fp / quo;    

            int exp_ = exp + hls::pow(2, exponent_size - 1) - 1;
            int man_ = man * hls::pow(2, mantissa_size);

            ap_uint<exponent_size> exp__ = exp_;
            ap_uint<mantissa_size> man__ = man_;

            sign = s;
            exponent = exp__;
            mantissa = man__; 
        }
    }
    */
    /*
    floatX(float c)
    {
        #pragma HLS INLINE

        // Reinterpret the float as an integer
        unsigned int bits = *reinterpret_cast<unsigned int*>(&c);
        //unsigned int* bitsPtr = (unsigned int*)&c;
        //unsigned int bits = *bitsPtr; // Dereference to get the raw bits

        // Extract the sign (1 bit)
        bool _sign = (bits >> 31) & 0x1;

        // Extract the exponent (8 bits, biased by 127)
        ap_uint<9> _exponent = (bits >> 23) & 0xFF;
        ap_int<9> __exponent = _exponent - hls::pow(2, 7);

        // Extract the mantissa (23 bits)
        ap_uint<23> _mantissa = bits & 0x7FFFFF; // Mask to get the last 23 bits

        sign = _sign;
        exponent = __exponent + hls::pow(2, exponent_size - 1);
        if(23 > mantissa_size)
            mantissa = _mantissa >> (23 - mantissa_size);
        else
            mantissa = _mantissa << (mantissa_size - 23);

    }
    */
    floatX(float c)
    {
        #pragma HLS INLINE

        ap_uint<32> bits = *reinterpret_cast<ap_uint<32>*>(&c);
        //unsigned int* bitsPtr = (unsigned int*)&c;
        //unsigned int bits = *bitsPtr; // Dereference to get the raw bits

        ap_uint<8> _exponent = bits(31, 23);
        ap_int<9> __exponent = _exponent - hls::pow(2, 7);

        sign = bits[31];
        exponent = __exponent + hls::pow(2, exponent_size - 1);

        if(23 > mantissa_size)
            mantissa = bits(22, 23 - mantissa_size);
        else
            mantissa = bits(22, 0);

    }
    /*
    operator float()
    {
        #pragma HLS INLINE

        float result;
        if(exponent == 0)
        {
            result = 0.0;
        }
        else 
        { 
            ap_uint<mantissa_size+1> un_mantissa = mantissa;
            un_mantissa[mantissa_size] = 1; //leading bit

            ap_int<exponent_size+1> un_exponent = exponent - (hls::pow(2, exponent_size - 1) - 1);

            result = float(un_mantissa)*hls::pow(2.0f, un_exponent - mantissa_size);
            if(sign == 1)
                result = -result;
        }
        return result;
    }
    */

    operator float()
    {
        #pragma HLS INLINE

        ap_uint<32> bits = 0;
        if(exponent != 0)
        { 
            bits(31, 31) = sign;
            
            ap_int<exponent_size+1> _exponent = exponent - hls::pow(2, exponent_size-1);
            ap_int<8> __exponent = _exponent + hls::pow(2, 7);

            bits(22 + exponent_size, 23) = __exponent;

            if(23 > mantissa_size)
                bits(22, 23 - mantissa_size) = mantissa;
            else
                bits(22, 0) = mantissa(mantissa_size-1, mantissa_size-23);
        }

        float fresult = *reinterpret_cast<float*>(&bits);
        return fresult;
    }

    floatX operator+(floatX c)
    {
        #pragma HLS INLINE

        floatX result;

        bool sign_1 = sign;
        ap_uint<mantissa_size+2> unnorm_mantissa_1 = mantissa;
        ap_uint<exponent_size> exponent_1 = exponent;

        if(exponent_1 == 0)
            unnorm_mantissa_1[mantissa_size] = 0; 
        else
            unnorm_mantissa_1[mantissa_size] = 1; //leading bit

        unnorm_mantissa_1[mantissa_size+1] = 0; //leave space for sign
        
        bool sign_2 = c.sign;
        ap_uint<mantissa_size+2> unnorm_mantissa_2 = c.mantissa;
        ap_uint<exponent_size> exponent_2 = c.exponent;

        if(exponent_2 == 0)
            unnorm_mantissa_2[mantissa_size] = 0;
        else
            unnorm_mantissa_2[mantissa_size] = 1; //leading bit

        unnorm_mantissa_2[mantissa_size+1] = 0; //leave space for sign

        if(exponent_1 < exponent_2)
        {
            bool aux_sign = sign_1;
            ap_uint<mantissa_size+2> aux_mantissa = unnorm_mantissa_1;
            ap_uint<exponent_size> aux_exponent = exponent_1;
            sign_1 = sign_2;
            unnorm_mantissa_1 = unnorm_mantissa_2;
            exponent_1 = exponent_2;
            sign_2 = aux_sign;
            unnorm_mantissa_2 = aux_mantissa;
            exponent_2 = aux_exponent;
        }

        ap_uint<exponent_size> exponent_diff = exponent_1 - exponent_2;

        ap_int<mantissa_size+2> signed_mantissa_1 = unnorm_mantissa_1;
        ap_int<mantissa_size+2> signed_mantissa_2 = unnorm_mantissa_2;

        if(sign_1 == 1)
            signed_mantissa_1 = -signed_mantissa_1;
        if(sign_2 == 1)
            signed_mantissa_2 = -signed_mantissa_2;

        const int add_added_zeros = 4;//mantissa_size;

        ap_int<mantissa_size+2+add_added_zeros> signed_mantissa_zeros_1;
        signed_mantissa_zeros_1(mantissa_size+1+add_added_zeros, add_added_zeros) = signed_mantissa_1;
        signed_mantissa_zeros_1(add_added_zeros-1, 0) = 0;
        ap_int<mantissa_size+2+add_added_zeros> signed_mantissa_zeros_2;
        signed_mantissa_zeros_2(mantissa_size+1+add_added_zeros, add_added_zeros) = signed_mantissa_2;
        signed_mantissa_zeros_2(add_added_zeros-1, 0) = 0;

        signed_mantissa_zeros_2 = signed_mantissa_zeros_2 >> exponent_diff;

        ap_int<mantissa_size+3+add_added_zeros> res_mantissa = signed_mantissa_zeros_1 + signed_mantissa_zeros_2;

        bool res_sign = 0;
        if(res_mantissa < 0)
        {
            res_sign = 1;
            res_mantissa = -res_mantissa;
        }

        int leading_zeros = count_leading_zeros(res_mantissa);

        ap_uint<exponent_size> res_exponent = exponent_1 + 2 - leading_zeros;
        ap_uint<mantissa_size+3+add_added_zeros> norm_mantissa = res_mantissa << (leading_zeros + 1);

        if(res_mantissa == 0)
        {
            result.sign = 0;
            result.exponent = 0;
            result.mantissa = 0;
        }
        else 
        {
            result.sign = res_sign;
            result.exponent = res_exponent;
            result.mantissa(mantissa_size-1, 0) = norm_mantissa(mantissa_size+2+add_added_zeros, 3+add_added_zeros);
        }
        return result;
    }

    floatX operator*(floatX c)
    {
        #pragma HLS INLINE

        floatX result;
        
        ap_uint<mantissa_size+1> unnorm_mantissa_1 = mantissa;
        if(exponent == 0)
            unnorm_mantissa_1[mantissa_size] = 0; //leading bit
        else
            unnorm_mantissa_1[mantissa_size] = 1; //leading bit

        ap_int<exponent_size> unnorm_exponent_1 = exponent - (hls::pow(2, exponent_size - 1) - 1);
        
        ap_uint<mantissa_size+1> unnorm_mantissa_2 = c.mantissa;
        if(c.exponent == 0)
            unnorm_mantissa_2[mantissa_size] = 0; //leading bit
        else
            unnorm_mantissa_2[mantissa_size] = 1; //leading bit

        ap_int<exponent_size> unnorm_exponent_2 = c.exponent - (hls::pow(2, exponent_size - 1) - 1);

        ap_uint<(mantissa_size+1)*2> res_mantissa = unnorm_mantissa_1*unnorm_mantissa_2;
        ap_int<exponent_size+1> res_exponent = unnorm_exponent_1 + unnorm_exponent_2;

        int leading_zeros = count_leading_zeros(res_mantissa);

        ap_uint<(mantissa_size+1)*2> norm_mantissa = res_mantissa << (leading_zeros + 1);
        ap_int<exponent_size+1> norm_exponent = res_exponent - leading_zeros + hls::pow(2, exponent_size - 1);

        if(res_mantissa == 0)
        {
            result.sign = 0;
            result.mantissa = 0;
            result.exponent = 0;
        }
        else
        {
            result.sign = sign ^ c.sign;
            result.mantissa(mantissa_size-1, 0) = norm_mantissa((mantissa_size+1)*2-1, mantissa_size+2);
            result.exponent = norm_exponent;
        }

        return result;
    }

    floatX operator-(floatX c)
    {
        #pragma HLS INLINE

        floatX negated = c;
        negated.sign = !negated.sign;
        floatX result = (*this)+ negated;
        return result;
    }

    floatX operator/(floatX c)
    {
        #pragma HLS INLINE

        floatX result;
        
        ap_uint<mantissa_size+1> unnorm_mantissa_1 = mantissa;
        if(exponent == 0)
            unnorm_mantissa_1[mantissa_size] = 0; //leading bit
        else
            unnorm_mantissa_1[mantissa_size] = 1; //leading bit

        ap_int<exponent_size> unnorm_exponent_1 = exponent - (hls::pow(2, exponent_size - 1) - 1);
        
        ap_uint<mantissa_size+1> unnorm_mantissa_2 = c.mantissa;
        if(c.exponent == 0)
            unnorm_mantissa_2[mantissa_size] = 0; //leading bit
        else
            unnorm_mantissa_2[mantissa_size] = 1; //leading bit

        ap_int<exponent_size> unnorm_exponent_2 = c.exponent - (hls::pow(2, exponent_size - 1) - 1);

        const int div_added_zeros = 32;//mantissa_size+2;

        ap_uint<mantissa_size+1+div_added_zeros> bigger_mantissa_1;// = unnorm_mantissa_1;
        bigger_mantissa_1(mantissa_size+div_added_zeros, div_added_zeros) = unnorm_mantissa_1(mantissa_size, 0);
        bigger_mantissa_1(div_added_zeros-1, 0) = 0;
        //bigger_mantissa_1 = bigger_mantissa_1 << div_added_zeros;

        ap_uint<mantissa_size+1+div_added_zeros> bigger_mantissa_2;
        bigger_mantissa_2(mantissa_size+div_added_zeros, mantissa_size+1) = 0;
        bigger_mantissa_2(mantissa_size, 0) = unnorm_mantissa_2(mantissa_size, 0);

        ap_uint<mantissa_size+1+div_added_zeros> res_mantissa = bigger_mantissa_1/bigger_mantissa_2;

        ap_int<exponent_size+1> res_exponent = unnorm_exponent_1 - unnorm_exponent_2;

        int leading_zeros = count_leading_zeros(res_mantissa);

        ap_uint<mantissa_size+1+div_added_zeros> norm_mantissa = res_mantissa << (leading_zeros + 1);
        ap_int<exponent_size+1> norm_exponent = res_exponent + mantissa_size - leading_zeros + hls::pow(2, exponent_size - 1) - 1;

        if(res_mantissa == 0)
        {
            result.sign = 0;
            result.mantissa = 0;
            result.exponent = 0;
        }
        else
        {
            result.sign = sign ^ c.sign;
            result.mantissa(mantissa_size-1, 0) = norm_mantissa(mantissa_size+div_added_zeros, div_added_zeros+1);
            result.exponent = norm_exponent;
        }

        return result;
    }

    void operator=(floatX c)
    {
        #pragma HLS INLINE
            
        sign = c.sign;
        exponent = c.exponent;
        mantissa = c.mantissa;
    }

    //ap_fixed<mantissa_size, mantissa_size, AP_RND> mantissa;
    //ap_fixed<exponent_size, exponent_size, AP_RND> exponent;

    bool sign;
    ap_uint<mantissa_size> mantissa;
    ap_uint<exponent_size> exponent;
};

