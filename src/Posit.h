#pragma once

#include "hls_math.h" //for things like hls::sin
#include "ap_int.h"   //to use ap_int vitis hls data type
#include "ap_fixed.h" //in case we want to use vitis hls fixed point data type

template <int nbits, int es>
class Posit
{
public:
    ap_uint<nbits> bits;

    Posit() : bits(0) {}
    explicit Posit(uint16_t b) : bits(b) {}
    // Removed float constructor to enforce pure posit arithmetic

    bool isZero() const { return bits == 0; }
    bool isNaR() const { return bits == (ap_uint<nbits>(1) << (nbits - 1)); }

    void decode(bool &sign, ap_uint<nbits-2-es> &k, ap_uint<es> &exponent, ap_fixed<nbits, 2> &fraction) const
    {
        if (isZero() || isNaR())
        {
            sign = false;
            k = 0;
            exponent = 0;
            fraction = 0;
            return;
        }

        // Posit sign (1 bit), r (variable bits), e (es bits), frac (variable bits)
        // to go from a Posit into a real number
        // x = -(1 - sign) * (u**r) * (2**e) * (1, frac)
        // r = 000001

        sign = bits[nbits - 1];

        int reg_len = 1;
        bool reg_bit = bits[nbits - 2];
        while (reg_len < nbits - 1 && bits[nbits - 2 - reg_len] == reg_bit)
        {
            reg_len++;
        }
        k = reg_bit ? (reg_len - 1) : (-reg_len);

        int exp_start = nbits - 3 - reg_len;
        exponent = bits(exp_start, exp_start - es);

        int frac_start = exp_start - es - 1;
        int frac_len = nbits - frac_start;

        fraction(nbits - 1) = 0;
        fraction(nbits - 2) = 1;
        fraction(nbits - 3, nbits - 3 - frac_len) = bits(frac_start, 0);
    }

    static Posit encode(bool sign, ap_uint<nbits-1-es> k, ap_uint<es> exponent, ap_fixed<nbits, 2> &fraction, int frac_len)
    {
        if (k > nbits - 2)
        {
            ap_uint<nbits> maxpos = (ap_uint<nbits>(1) << (nbits - 1)) - 1;

            ap_int<nbits + 2> sval;
            if (sign)
            {
                sval = -ap_int<nbits + 2>(maxpos);
            }
            else
            {
                sval = ap_int<nbits + 2>(maxpos);
            }

            ap_uint<nbits> val = sval.range(nbits - 1, 0);
            return Posit(uint16_t(val));
        }
        if (k < -(nbits - 2))
        {
            return Posit(uint16_t(0));
        }

        ap_uint<nbits> result = 0;

        int reg_len = std::abs(k) + 1;
        if (reg_len > nbits - 1)
            reg_len = nbits - 1;

        if (k >= 0)
        {
            for (int i = 0; i < reg_len; ++i)
                result.set(nbits - 2 - i, 1);
            if (nbits - 2 - reg_len >= 0)
                result.set(nbits - 2 - reg_len, 0);
        }
        else
        {
            for (int i = 0; i < reg_len; ++i)
                result.set(nbits - 2 - i, 0);
            if (nbits - 2 - reg_len >= 0)
                result.set(nbits - 2 - reg_len, 1);
        }

        int exponent_start = nbits - 2 - reg_len;
        result(exponent_star, exponent_start - es) = exponent;

        int fraction_start = exponent_start - es - 1;
        int fraction_len = nbits - exponent_start - es - 1;
        result(fraction_start, fraction_start - fraction_len) = fraction(nbits - 3, nbits - 3 - fraction_len);

        result[nbits - 1] = sign;

        return Posit(uint16_t(result));
    }

    static ap_fixed<nbits, 2> round_fraction(ap_fixed<nbits, 2> value, int shift, int frac_bits)
    {
        if (shift <= 0)
            return value << (-shift);
        ap_fixed<nbits, 2> mask = (1ull << shift) - 1;
        ap_fixed<nbits, 2> halfway = 1ull << (shift - 1);
        ap_fixed<nbits, 2> lsb = (value >> shift) & 1;
        bool round = ((value & mask) > halfway) ||
                     ((value & mask) == halfway && lsb);
        return (value >> shift) + round;
    }

    Posit operator+(const Posit &rhs) const
    {
        if (isNaR() || rhs.isNaR())
            return Posit(uint16_t(1 << (nbits - 1)));
        if (isZero())
            return rhs;
        if (rhs.isZero())
            return *this;

        bool signA, signB;
        int kA, kB, expA, expB;
        ap_fixed<nbits, 2> fracA, fracB;
        int fracLenA, fracLenB;
        decode(signA, kA, expA, fracA, fracLenA);
        rhs.decode(signB, kB, expB, fracB, fracLenB);

        int scaleA = kA * (1 << es) + expA;
        int scaleB = kB * (1 << es) + expB;

        ap_fixed<nbits, 2> mantA = (ap_fixed<nbits, 2>(1) << fracLenA) | fracA;
        ap_fixed<nbits, 2> mantB = (ap_fixed<nbits, 2>(1) << fracLenB) | fracB;

        int scaleDiff = scaleA - scaleB;
        if (scaleDiff > 0)
        {
            if (scaleDiff > 64)
                mantB = 0;
            else
                mantB = mantB >> scaleDiff;
            scaleB = scaleA;
        }
        else if (scaleDiff < 0)
        {
            int shift = -scaleDiff;
            if (shift > 64)
                mantA = 0;
            else
                mantA = mantA >> shift;
            scaleA = scaleB;
        }

        ap_fixed<nbits, 2> resMant;
        bool resSign;
        if (signA == signB)
        {
            resMant = mantA + mantB;
            resSign = signA;
        }
        else
        {
            if (mantA >= mantB)
            {
                resMant = mantA - mantB;
                resSign = signA;
            }
            else
            {
                resMant = mantB - mantA;
                resSign = signB;
            }
            if (resMant == 0)
                return Posit(0);
        }

        int resFracLen = std::max(fracLenA, fracLenB) + 1;
        int leading_zeros = 0;
        while (((resMant & (ap_fixed<nbits, 2>(1) << (resFracLen))) == 0) && resFracLen > 0)
        {
            resMant <<= 1;
            leading_zeros++;
            resFracLen--;
        }

        int resScale = scaleA - leading_zeros;

        int max_exponent = (1 << es) - 1;
        int res_k = resScale >> es;
        int res_exp = resScale & max_exponent;

        int frac_bits = nbits - 1 - (std::abs(res_k) + 1) - es;
        if (frac_bits < 0)
            frac_bits = 0;

        int shift = resFracLen - frac_bits;
        ap_fixed<nbits, 2> rounded_frac = round_fraction(resMant, shift, frac_bits);

        if (rounded_frac >= (ap_fixed<nbits, 2>(1) << frac_bits))
        {
            rounded_frac >>= 1;
            res_exp++;
            if (res_exp > max_exponent)
            {
                res_exp = 0;
                res_k++;
            }
        }

        return encode(resSign, res_k, res_exp, rounded_frac, frac_bits);
    }

    Posit operator-(const Posit &rhs) const
    {
        // Subtraction: only flip the sign of rhs
        bool sign_rhs = !rhs.isZero();
        ap_fixed<nbits, 2> default_fraction = 0;
        return *this + encode(sign_rhs, 0, 0, default_fraction, 0);
    }

    Posit operator*(const Posit &rhs) const
    {
        if (isNaR() || rhs.isNaR())
            return Posit(uint16_t(1 << (nbits - 1)));
        if (isZero() || rhs.isZero())
            return Posit(uint16_t(0));

        // Posit sign (1 bit), r (variable bits), e (es bits), frac (variable bits)
        // to go from a Posit into a real number
        // x = -(1 - sign_x) * (u**r_x) * (2**e_x) * (1, frac_x)
        // y = -(1 - sign_y) * (u**r_y) * (2**e_y) * (1, frac_y)
        // x*y = (1 - sign_x) * (u**r_x) * (2**e_x) * (1, frac_x) * (1 - sign_y) * (u**r_y) * (2**e_y) * (1, frac_y)
        // x*y = (u**(r_x + r_y)) * 2**(e_x + e_y) * (1, frac_x) * (1, frac_y)

        bool signA, signB;
        ap_uint<nbits - 2 - es> kA, kB, 
        ap_uint<es> expA, expB;
        ap_fixed<nbits, 2> fracA, fracB;

        decode(signA, kA, expA, fracA);
        rhs.decode(signB, kB, expB, fracB);

        bool res_sign = signA ^ signB;
        ap_uint<nbits - 2 - es> res_k = kA + kB;
        ap_uint<es+1> res_exp = expA + expB;

        ap_fixed<nbits, 2> res_frac = mantA * mantB;

        /*
        int product_len = fracLenA + fracLenB + 1;
        if (product & (ap_fixed<nbits, 2>(1) << product_len))
        {
            product >>= 1;
            res_exp++;
        }

        int max_exponent = (1 << es) - 1;
        if (res_exp > max_exponent)
        {
            res_exp -= (max_exponent + 1);
            res_k++;
        }

        int frac_bits = nbits - 1 - (std::abs(res_k) + 1) - es;
        if (frac_bits < 0)
            frac_bits = 0;
        int shift = product_len - frac_bits;
        ap_fixed<nbits, 2> res_frac = round_fraction(product, shift, frac_bits);

        if (res_frac >= (ap_fixed<nbits, 2>(1) << frac_bits))
        {
            res_frac >>= 1;
            res_exp++;
            if (res_exp > max_exponent)
            {
                res_exp = 0;
                res_k++;
            }
        }
        */

        return encode(res_sign, res_k, res_exp, res_frac);
    }

    Posit operator/(const Posit &rhs) const
    {
        if (rhs.isZero() || isNaR() || rhs.isNaR())
            return Posit(uint16_t(1 << (nbits - 1)));
        if (isZero())
            return Posit(0);

        bool signA, signB;
        int kA, kB, expA, expB;
        ap_fixed<nbits, 2> fracA, fracB;
        int fracLenA, fracLenB;
        decode(signA, kA, expA, fracA, fracLenA);
        rhs.decode(signB, kB, expB, fracB, fracLenB);

        bool res_sign = signA ^ signB;
        int res_k = kA - kB;
        int res_exp = expA - expB;

        ap_fixed<nbits, 2> mantA = (ap_fixed<nbits, 2>(1) << fracLenA) | fracA;
        ap_fixed<nbits, 2> mantB = (ap_fixed<nbits, 2>(1) << fracLenB) | fracB;

        int shift = 63 - fracLenA;
        ap_fixed<nbits, 2> dividend = mantA << shift;
        ap_fixed<nbits, 2> quotient = dividend / mantB;

        int quotient_len = 64;

        while (((quotient & (ap_fixed<nbits, 2>(1) << (quotient_len - 1))) == 0) && quotient_len > 0)
        {
            quotient <<= 1;
            quotient_len--;
        }

        int max_exponent = (1 << es) - 1;
        if (res_exp < 0)
        {
            while (res_exp < 0 && res_k > -(nbits - 2))
            {
                res_exp += max_exponent + 1;
                res_k--;
            }
            if (res_exp < 0)
            {
                return Posit(0);
            }
        }

        int frac_bits = nbits - 1 - (std::abs(res_k) + 1) - es;
        if (frac_bits < 0)
            frac_bits = 0;

        int round_shift = quotient_len - frac_bits;
        ap_fixed<nbits, 2> res_frac = round_fraction(quotient, round_shift, frac_bits);

        if (res_frac >= (ap_fixed<nbits, 2>(1) << frac_bits))
        {
            res_frac >>= 1;
            res_exp++;
            if (res_exp > max_exponent)
            {
                res_exp = 0;
                res_k++;
            }
        }

        return encode(res_sign, res_k, res_exp, res_frac, frac_bits);
    }

    // For debugging only: convert posit to float (not used internally)
    float to_float() const
    {
        if (isZero())
            return 0.0f;
        if (isNaR())
            return std::numeric_limits<float>::quiet_NaN();

        bool sign;
        int k, exponent;
        ap_fixed<nbits, 2> frac;
        int frac_len;
        decode(sign, k, exponent, frac, frac_len);

        int scale = k * (1 << es) + exponent;
        double value = 1.0;
        for (int i = 0; i < frac_len; ++i)
        {
            if ((frac >> (frac_len - 1 - i)) & 1)
            {
                value += std::ldexp(1.0, -(i + 1));
            }
        }
        value = std::ldexp(value, scale);
        return sign ? float(-value) : float(value);
    }
};
