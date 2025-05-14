#pragma once

#include "hls_math.h" //for things like hls::sin
#include "ap_int.h"   //to use ap_int vitis hls data type
#include "ap_fixed.h" //in case we want to use vitis hls fixed point data type

template <int nbits, int es>
class Posit
{
public:
    static constexpr int max_k_size = nbits - 1; // hls::log2(nbits - 1);
    static constexpr int max_frac_size = nbits - 3 - es + 1;

    struct unpacked_t
    {
        bool sign;
        // the max amount of bits for r is nbits-1 bits, nbits-2 bits beeing 0 (or 1), and the last beeing 1 (or 0)
        // k is the amount of counted bits
        // which can be stored in log2(nbits - 2) bits
        ap_int<max_k_size> k;
        ap_uint<es> exp;
        // the max amount of bits for frac is nbits - 1 (sign) - 2 (min bits for k) - es;
        ap_ufixed<max_frac_size, 1> frac;
    };

    Posit()
    {
    }

    Posit(const Posit &other)
    {
        bits_ = other.bits_;
    }

    Posit &operator=(const Posit &other)
    {
        if (this != &other)
        {
            bits_ = other.bits_;
        }
        return *this;
    }

    Posit(float c)
    {
#pragma HLS INLINE

        ap_uint<32> bits = *reinterpret_cast<ap_uint<32> *>(&c);
        // unsigned int* bitsPtr = (unsigned int*)&c;
        // unsigned int bits = *bitsPtr; // Dereference to get the raw bits

        bool fsign = bits[31];
        // remove bias from floating point exponent
        ap_int<9> fexponent = bits(30, 23) - hls::pow(2, 7) + 1;
        // get mantisa from floating point
        ap_ufixed<24, 1> fmantissa;
        fmantissa[23] = 1;
        fmantissa(22, 0) = bits(22, 0);

        bool psign = fsign;
        ap_ufixed<max_frac_size, 1> pmantissa = fmantissa;

        // get k and e from floating point exponent
        // init k = 0, and e = exponent
        ap_int<max_k_size> pk = 0;
        ap_int<9> pexponent = fexponent;

        // force e to be in the allowed range (0 - 2**es-1)
        while (pexponent > hls::pow(2, es) - 1)
        {
            pexponent -= hls::pow(2, es);
            pk++;
        }

        while (pexponent < 0)
        {
            pexponent += hls::pow(2, es);
            pk--;
        }

        if (c == 0.0f)
        {
            psign = 0;
            pmantissa = 0;
            pexponent = 0;
            pk = 0;
        }

        unpacked_t unpacked;
        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.exp = pexponent;
        unpacked.k = pk;

        encode(unpacked);
    }

    operator float()
    {
#pragma HLS INLINE

        unpacked_t unpacked = decode();

        ap_uint<32> float_bits = 0;

        float_bits(31, 31) = unpacked.sign;

        ap_uint<8> exponent;

        if (unpacked.frac[max_frac_size - 1] == 0)
            exponent = 0;
        else
            exponent = unpacked.exp + unpacked.k * hls::pow(2, es) + hls::pow(2, 7) - 1;

        float_bits(30, 23) = exponent;

        if (23 >= max_frac_size - 1)
            float_bits(22, 22 - max_frac_size + 2) = unpacked.frac(max_frac_size - 2, 0);
        else
            float_bits(22, 0) = unpacked.frac(max_frac_size - 2, max_frac_size - 2 - 22);

        float fresult = *reinterpret_cast<float *>(&float_bits);
        return fresult;
    }

    Posit operator+(const Posit &rhs) const
    {
        unpacked_t in1 = decode();
        unpacked_t in2 = rhs.decode();

        // set biggest posit to be in1
        ap_int<max_k_size + es> diff_texp = (in1.k - in2.k) * hls::pow(2, es) + in1.exp - in2.exp;

        if (diff_texp < 0)
        {
            unpacked_t paux = in1;
            in1 = in2;
            in2 = paux;

            diff_texp = -diff_texp;
        }

        // k and exp are from the biggest number
        ap_int<max_k_size> k = in1.k;
        ap_int<es * 2> exp = in1.exp;

        // add the sign back into the fraction, so that we can do the sum
        ap_fixed<max_frac_size + 2, 2> frac1 = in1.frac;
        ap_fixed<max_frac_size + 2, 2> frac2 = in2.frac;

        if (in1.sign)
            frac1 = -frac1;
        else
            frac1 = frac1;

        if (in2.sign)
            frac2 = -frac2;
        else
            frac2 = frac2;

        // shift left in2, so that both have the same exponent
        for (int i = 0; i < diff_texp; i++)
            frac2 = frac2 >> 1;

        // do addition
        ap_fixed<max_frac_size + 3, 3> frac = frac1 + frac2;

        // get sign and remove sign from frac
        bool sign;

        if (frac < 0)
        {
            frac = -frac;
            sign = 1;
        }
        else
        {
            // frac = frac;
            sign = 0;
        }

        // normalize
        if (frac == 0)
        {
            exp = 0;
            k = 0;
        }
        else
        {
            while (frac >= 2)
            {
                frac = frac >> 1;
                exp++;
            }

            while (frac < 1)
            {
                frac = frac << 1;
                exp--;
            }

            while (exp >= hls::pow(2, es))
            {
                exp -= hls::pow(2, es);
                k++;
            }

            while (exp < 0)
            {
                exp += hls::pow(2, es);
                k--;
            }
        }

        unpacked_t out;
        out.sign = sign;
        out.k = k;
        out.exp = exp;
        out.frac = frac;

        Posit output;
        output.encode(out);

        return output;
    }

    Posit operator-(const Posit &rhs) const
    {
        Posit neg = rhs;
        neg.bits_[nbits - 1] = !neg.bits_[nbits - 1];
        Posit result = (*this) + neg;
        return result;
    }

    Posit operator*(const Posit &rhs) const
    {
        unpacked_t in1 = decode();
        unpacked_t in2 = rhs.decode();

        bool sign = in1.sign ^ in2.sign;
        ap_int<max_k_size + 1> k = in1.k + in2.k;
        ap_uint<es + 1> exp = in1.exp + in2.exp;
        ap_ufixed<max_frac_size * 2, 2> frac = in1.frac * in2.frac;

        // normalize
        if (frac >= 2)
        {
            frac = frac >> 1;
            exp++;
        }

        if (exp >= hls::pow(2, es))
        {
            exp -= hls::pow(2, es);
            k++;
        }

        /*
        if (exp < 0)
        {
            exp += hls::pow(2, es);
            k--;
        }
        */

        if (frac(max_frac_size * 2 - 1, max_frac_size * 2 - 2) == 0)
        {
            sign = 0;
            k = 0;
            exp = 0;
            frac = 0;
        }

        unpacked_t out;
        out.sign = sign;
        out.k = k;
        out.exp = exp;
        out.frac = frac;

        Posit output;
        output.encode(out);

        return output;
    }

    Posit operator/(const Posit &rhs) const
    {
        unpacked_t in1 = decode();
        unpacked_t in2 = rhs.decode();

        bool sign = in1.sign ^ in2.sign;
        ap_int<max_k_size> k = in1.k - in2.k;
        ap_int<es + 1> exp = in1.exp - in2.exp;
        ap_ufixed<max_frac_size * 2, 1> frac = in1.frac / in2.frac;

        // normalize
        if (frac < 1)
        {
            frac = frac << 1;
            exp--;
        }

        if (exp < 0)
        {
            exp += hls::pow(2, es);
            k--;
        }

        if (frac[max_frac_size * 2 - 1] == 0)
        {
            sign = 0;
            k = 0;
            exp = 0;
            frac = 0;
        }

        unpacked_t out;
        out.sign = sign;
        out.k = k;
        out.exp = exp;
        out.frac = frac;

        Posit output;
        output.encode(out);

        return output;
    }

private:
    unpacked_t decode() const
    {
        // Posit sign (1 bit), r (variable bits), e (es bits), frac (variable bits)
        // to go from a Posit into a real number
        // x = -(1 - sign) * (u**k) * (2**e) * (1, frac)
        // with u = 2**(2**es)

        unpacked_t unpacked;

        // sign bit
        unpacked.sign = bits_[nbits - 1];

        // count identical bits
        int reg_len = 1;
        bool reg_bit = bits_[nbits - 2];
        while (reg_len < nbits - 1 && bits_[nbits - 2 - reg_len] == reg_bit)
        {
            reg_len++;
        }

        // k is just reg_len (or -reg_len if leading bit is 1)
        if (reg_len == nbits - 1)
            unpacked.k = 0;
        else
            unpacked.k = reg_bit == 0 ? (reg_len - 1) : (-reg_len);

        // exponent bits
        int exp_start = nbits - 3 - reg_len;
        int exp_end = hls::max(exp_start - es + 1, 0);
        int exp_len = exp_start - exp_end + 1;

        if (exp_len > 0)
            unpacked.exp(exp_len - 1, 0) = bits_(exp_start, exp_end);
        else
            unpacked.exp = 0;

        // fraction bits
        int frac_start = exp_end - 1;
        int frac_end = 0;
        int frac_len = frac_start - frac_end + 1;

        // add leading 1
        if (reg_len == nbits - 1)
            unpacked.frac[max_frac_size - 1] = 0;
        else
            unpacked.frac[max_frac_size - 1] = 1;

        // if there is more bits to read for the fraction, read them
        if (frac_len > 0)
            unpacked.frac(max_frac_size - 2, max_frac_size - 2 - frac_len + 1) = bits_(frac_start, frac_end);
        else
            unpacked.frac(max_frac_size - 2, 0) = 0;

        return unpacked;
    }

    void encode(const unpacked_t &unpacked)
    {
        // k bits
        bool reg_bit;
        int reg_len;

        // if the integer bit in the fraction is 0, then the whole number is zero
        if (unpacked.frac[max_frac_size - 1] == 0)
        {
            bits_[nbits - 1] = 0;
            reg_bit = 0;
            reg_len = nbits - 1;
        }
        else
        {
            bits_[nbits - 1] = unpacked.sign;
            reg_bit = unpacked.k >= 0 ? 0 : 1;
            reg_len = unpacked.k >= 0 ? int(unpacked.k + 1) : int(-unpacked.k);
        }

        int reg_start = nbits - 2;

        for (int i = 0; i < reg_len; i++)
        {
            bits_[reg_start - i] = reg_bit;
        }

        int reg_end = reg_start - reg_len;

        if (reg_end >= 0)
        {
            bits_[reg_end] = !reg_bit;
        }

        // exponent bits
        int exp_start = nbits - 3 - reg_len;
        int exp_end = hls::max(exp_start - es + 1, 0);
        int exp_len = exp_start - exp_end + 1;

        if (exp_len > 0)
            bits_(exp_start, exp_end) = unpacked.exp(exp_len - 1, 0);

        // fraction bits
        int frac_start = exp_end - 1;
        int frac_end = 0;
        int frac_len = frac_start - frac_end + 1;

        if (frac_len > 0)
            bits_(frac_start, frac_end) = unpacked.frac(max_frac_size - 2, max_frac_size - 1 - frac_len);
    }

    ap_uint<nbits> bits_;
};
