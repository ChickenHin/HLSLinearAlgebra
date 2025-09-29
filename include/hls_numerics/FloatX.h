#pragma once

#include "hls_math.h"
#include "ap_int.h"
#include "ap_fixed.h"

#include "common.h"

template <int ebits, int fbits>
class FloatXUnpacked
{
public:
    template <int nbits, int es>
    void decode(const ap_uint<nbits> &bits)
    {
#pragma HLS INLINE

        bool sign = bits[nbits - 1];
        ap_uint<es> exp = bits(nbits - 2, nbits - 1 - es);
        ap_uint<nbits - 1 - es> frac = bits(nbits - 2 - es, 0);

        bool isZero = false;
        if (bits == 0)
            isZero = true;

        sign_ = sign;

        // exponent bits
        if (isZero)
            exp_ = 0;
        else
            // unpacked.exp = exp - hls::pow(2, es - 1) + 1;
            exp_ = exp - (1 << (es - 1)) + 1;

        // fraction bits
        // add leading 1
        if (isZero)
            frac_[fbits - 1] = 0;
        else
            frac_[fbits - 1] = 1;

        frac_(fbits - 2, 0) = frac;
    }

    template <int nbits, int es>
    ap_uint<nbits> encode() const
    {
#pragma HLS INLINE

        ap_uint<nbits> bits;

        ap_int<es + 1> exp;
        if (frac_ == 0)
            exp = 0;
        else
            // exp = unpacked.exp + hls::pow(2, es - 1) - 1;
            exp = exp_ + (1 << (es - 1)) - 1;

        bits[nbits - 1] = sign_;
        bits(nbits - 2, nbits - 1 - es) = exp(es - 1, 0);
        bits(nbits - 2 - es, 0) = frac_(fbits - 2, 0);

        return bits;
    }

    FloatXUnpacked operator+(const FloatXUnpacked &rhs) const
    {
#pragma HLS INLINE

        // set biggest posit to be in1
        ap_int<ebits + 1> diff_texp = exp_ - rhs.exp_;

        ap_fixed<fbits + 1, 2> frac1 = frac_;
        ap_fixed<fbits + 1, 2> frac2 = rhs.frac_;

        ap_uint<ebits> exp;
        bool sign;

        if (diff_texp >= 0)
        {
            exp = exp_;
            frac2 = frac2 >> diff_texp;

            // sign of output is sign of largest number
            sign = sign_;
            // check if we have to add or substract
            if (sign_ != rhs.sign_)
                frac2 = -frac2;
        }
        else
        {
            exp = rhs.exp_;
            frac1 = frac1 >> -diff_texp;

            // sign of output is sign of largest number
            sign = rhs.sign_;
            // check if we have to add or substract
            if (sign_ != rhs.sign_)
                frac1 = -frac1;
        }

        // do addition (result is sure to be positive)
        ap_fixed<fbits + 3, 3> frac = frac1 + frac2;

        // normalize
        if (frac == 0)
        {
            exp = 0;
        }
        else
        {
            int shift = count_leading_simbol(frac, 0) - 2;
            if (shift > 0)
            {
                frac = frac << shift;
                exp = exp - shift;
            }
            else if (shift < 0)
            {
                frac = frac >> -shift;
                exp = exp + -shift;
            }
        }

        FloatXUnpacked out;
        out.sign_ = sign;
        out.exp_ = exp;
        out.frac_ = frac;

        return out;
    }

    FloatXUnpacked operator*(const FloatXUnpacked &rhs) const
    {
#pragma HLS INLINE

        bool sign = sign_ ^ rhs.sign_;
        ap_int<ebits + 1> exp = exp_ + rhs.exp_;
        ap_fixed<fbits * 2, 2> frac = frac_ * rhs.frac_;

        // normalize
        if (frac >= 2)
        {
            frac = frac >> 1;
            exp++;
        }

        // round
        ap_ufixed<fbits * 2, 2> rfrac = round_to(frac, fbits - 1);

        // normalize (again)
        if (rfrac >= 2)
        {
            rfrac = rfrac >> 1;
            exp++;
        }
        /*
        if (rfrac == 0)
        {
            exp = 0;
            rfrac = 0;
        }
        */

        FloatXUnpacked out;
        out.sign = sign;
        out.exp = exp;
        out.frac = rfrac;

        return out;
    }

    FloatXUnpacked operator/(const FloatXUnpacked &rhs) const
    {
#pragma HLS INLINE

        bool sign = sign_ ^ rhs.sign;
        ap_int<ebits + 1> exp = exp_ - rhs.exp_;
        ap_ufixed<fbits * 2, 1> frac1 = frac_;
        ap_ufixed<fbits * 2, 1> frac2 = rhs.frac_;
        ap_ufixed<fbits * 2, 1> frac;

        if (rhs.frac_ != 0)
            frac = frac1 / frac2;

        // normalize
        if (frac < 1)
        {
            frac = frac << 1;
            exp--;
        }

        ap_ufixed<fbits * 2, 2> rfrac = round_to(frac, fbits - 1);

        // normalize
        if (rfrac < 1)
        {
            rfrac = rfrac << 1;
            exp--;
        }

        if (rfrac == 0)
        {
            exp = 0;
            frac = 0;
        }

        FloatXUnpacked out;
        out.sign = sign;
        out.exp = exp;
        out.frac = rfrac;

        return out;
    }

private:
    bool sign_;
    ap_int<ebits + 1> exp_;
    ap_ufixed<fbits + 1, 1> frac_;
};

template <int nbits, int es>
class FloatX
{
public:
    static constexpr int exp_size = es;
    static constexpr int frac_size = nbits - es - 1;

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

        FloatXUnpacked<8, 23> unpacked;
        unpacked.template decode<32, 8>(bits);
        bits_ = unpacked.template encode<nbits, es>();
    }

    FloatX(double c)
    {
#pragma HLS INLINE

        ap_uint<64> bits = *reinterpret_cast<ap_uint<64> *>(&c);
        // unsigned int* bitsPtr = (unsigned int*)&c;
        // unsigned int bits = *bitsPtr; // Dereference to get the raw bits

        FloatXUnpacked<11, 52> unpacked;
        unpacked.template decode<64, 11>(bits);
        bits_ = unpacked.template encode<nbits, es>();
    }

    operator float() const
    {
#pragma HLS INLINE

        FloatXUnpacked<8, 23> unpacked;
        unpacked.template decode<nbits, es>(bits_);
        ap_uint<32> bits = unpacked.template encode<32, 8>();

        float fresult = *reinterpret_cast<float *>(&bits);
        return fresult;
    }

    operator double() const
    {
#pragma HLS INLINE

        FloatXUnpacked<11, 52> unpacked;
        unpacked.template decode<nbits, es>(bits_);
        ap_uint<64> bits = unpacked.template encode<64, 11>();

        double fresult = *reinterpret_cast<double *>(&bits);
        return fresult;
    }

    FloatX operator+(const FloatX &rhs) const
    {
#pragma HLS INLINE

        FloatXUnpacked<exp_size, frac_size> in1;
        in1.template decode<nbits, es>(bits_);
        FloatXUnpacked<exp_size, frac_size> in2;
        in2.template decode<nbits, es>(rhs.bits_);

        FloatXUnpacked<exp_size, frac_size> res = in1 + in2;

        FloatX out;
        out.bits_ = res.template encode<nbits, es>();

        return out;
    }

    FloatX operator-(const FloatX &rhs) const
    {
#pragma HLS INLINE

        FloatX neg = rhs;
        neg.bits_[nbits - 1] = !neg.bits_[nbits - 1];
        FloatX result = (*this) + neg;
        return result;
    }

    FloatX operator*(const FloatX &rhs) const
    {
#pragma HLS INLINE

        FloatXUnpacked<exp_size, frac_size> in1;
        in1.template decode<nbits, es>(bits_);
        FloatXUnpacked<exp_size, frac_size> in2;
        in2.template decode<nbits, es>(rhs.bits_);

        FloatXUnpacked<exp_size, frac_size> res = in1 * in2;

        FloatX out;
        out.bits_ = res.template encode<nbits, es>();
        return out;
    }

    FloatX operator/(const FloatX &rhs) const
    {
#pragma HLS INLINE

        FloatXUnpacked<exp_size, frac_size> in1;
        in1.template decode<nbits, es>(bits_);
        FloatXUnpacked<exp_size, frac_size> in2;
        in2.template decode<nbits, es>(rhs.bits_);

        FloatXUnpacked<exp_size, frac_size> res = in1 / in2;

        FloatX out;
        out.bits_ = res.template encode<nbits, es>();

        return out;
    }

private:
    ap_uint<nbits> bits_;
};
