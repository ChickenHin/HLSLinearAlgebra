#pragma once

#include "hls_math.h" //for things like hls::sin
#include "ap_int.h"   //to use ap_int vitis hls data type
#include "ap_fixed.h" //in case we want to use vitis hls fixed point data type

template <int nbits, int es>
class FloatX
{
public:
    static constexpr int exp_size = es + 1;
    static constexpr int frac_size = nbits - es;

    struct unpacked_t
    {
        // lets unpack to a sign, a signed exp and a unsigned fixed point for the fraction
        bool sign;
        ap_int<exp_size> exp;
        ap_ufixed<frac_size, 1> frac;
    };

    FloatX()
    {
    }

    FloatX(const FloatX &other)
    {
        bits_ = other.bits_;
    }

    FloatX &operator=(const FloatX &other)
    {
        if (this != &other)
        {
            bits_ = other.bits_;
        }
        return *this;
    }

    FloatX(float c)
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

        unpacked_t unpacked;

        unpacked.sign = fsign;
        unpacked.exp = fexponent;
        unpacked.frac = fmantissa;

        if (c == 0.0f)
        {
            unpacked.sign = 0;
            unpacked.exp = 0;
            unpacked.frac = 0;
        }

        encode(unpacked);
    }

    operator float()
    {
#pragma HLS INLINE

        unpacked_t unpacked = decode();

        ap_uint<32> float_bits = 0;

        float_bits(31, 31) = unpacked.sign;

        ap_uint<8> exponent;
        if (unpacked.frac == 0)
            exponent = 0;
        else
            exponent = unpacked.exp + hls::pow(2, 7) - 1;

        float_bits(30, 23) = exponent;

        if (23 >= nbits - 1 - es)
            float_bits(22, 22 - nbits + es + 2) = unpacked.frac(frac_size - 2, 0);
        else
            float_bits(22, 0) = unpacked.frac(frac_size - 2, frac_size - 2 - 22);

        float fresult = *reinterpret_cast<float *>(&float_bits);
        return fresult;
    }

    FloatX operator+(const FloatX &rhs) const
    {
        unpacked_t in1 = decode();
        unpacked_t in2 = rhs.decode();

        // set biggest posit to be in1
        ap_int<exp_size> diff_texp = in1.exp - in2.exp;

        if (diff_texp < 0)
        {
            unpacked_t paux = in1;
            in1 = in2;
            in2 = paux;

            diff_texp = -diff_texp;
        }

        // exp are from the biggest number
        ap_int<exp_size> exp = in1.exp;

        // add the sign back into the fraction, so that we can do the sum
        ap_fixed<frac_size + 1, 2> frac1 = in1.frac;
        ap_fixed<frac_size + 1, 2> frac2 = in2.frac;

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
        ap_fixed<frac_size + 2, 3> frac = frac1 + frac2;

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
        }

        unpacked_t out;
        out.sign = sign;
        out.exp = exp;
        out.frac = frac;

        FloatX output;
        output.encode(out);

        return output;
    }

    FloatX operator-(const FloatX &rhs) const
    {
        FloatX neg = rhs;
        neg.bits_[nbits - 1] = !neg.bits_[nbits - 1];
        FloatX result = (*this) + neg;
        return result;
    }

    FloatX operator*(const FloatX &rhs) const
    {
        unpacked_t in1 = decode();
        unpacked_t in2 = rhs.decode();

        bool sign = in1.sign ^ in2.sign;
        ap_int<exp_size + 1> exp = in1.exp + in2.exp;
        ap_ufixed<frac_size * 2, 2> frac = in1.frac * in2.frac;

        // normalize
        if (frac >= 2)
        {
            frac = frac >> 1;
            exp++;
        }

        if (frac == 0)
        {
            exp = 0;
            frac = 0;
        }

        unpacked_t out;
        out.sign = sign;
        out.exp = exp;
        out.frac = frac;

        FloatX output;
        output.encode(out);

        return output;
    }

    FloatX operator/(const FloatX &rhs) const
    {
        unpacked_t in1 = decode();
        unpacked_t in2 = rhs.decode();

        bool sign = in1.sign ^ in2.sign;
        ap_int<exp_size> exp = in1.exp - in2.exp;
        ap_ufixed<frac_size * 2, 1> frac1 = in1.frac;
        ap_ufixed<frac_size * 2, 1> frac2 = in2.frac;
        ap_ufixed<frac_size * 2, 1> frac = frac1 / frac2;

        // normalize
        if (frac < 1)
        {
            frac = frac << 1;
            exp--;
        }

        if (frac == 0)
        {
            exp = 0;
            frac = 0;
        }

        unpacked_t out;
        out.sign = sign;
        out.exp = exp;
        out.frac = frac;

        FloatX output;
        output.encode(out);

        return output;
    }

private:
    unpacked_t decode() const
    {
        unpacked_t unpacked;

        // sign bit
        bool sign = bits_[nbits - 1];
        ap_uint<es> exp = bits_(nbits - 2, nbits - 1 - es);
        ap_uint<nbits - 1 - es> frac = bits_(nbits - 2 - es, 0);

        unpacked.sign = sign;
        // exponent bits
        unpacked.exp = exp - hls::pow(2, es - 1) + 1;

        // fraction bits
        // add leading 1
        unpacked.frac[frac_size - 1] = 1;
        unpacked.frac(frac_size - 2, 0) = frac;

        return unpacked;
    }

    void encode(const unpacked_t &unpacked)
    {
        bool sign = unpacked.sign;

        ap_int<exp_size> exp;
        if (unpacked.frac == 0)
            exp = 0;
        else
            exp = unpacked.exp + hls::pow(2, es - 1) - 1;

        ap_ufixed<frac_size, 1> frac = unpacked.frac;

        bits_[nbits - 1] = sign;
        bits_(nbits - 2, nbits - 1 - es) = exp(es - 1, 0);
        bits_(nbits - 2 - es, 0) = frac(frac_size - 2, 0);
    }

    ap_uint<nbits> bits_;
};
