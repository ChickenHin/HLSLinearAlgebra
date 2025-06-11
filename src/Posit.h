#pragma once

#include "hls_math.h" //for things like hls::sin
#include "ap_int.h"   //to use ap_int vitis hls data type
#include "ap_fixed.h" //in case we want to use vitis hls fixed point data type

namespace detail
{
    // 2^k for *integer* k
    constexpr float pow2(int k)
    {
        // #pragma HLS INLINE
        return k >= 0 ? (1u << k) : 1.0f / (1u << (-k));
    }

    // Compile‑time ceil(log2(N))
    // Usage: clog2<17>::value == 5 (because 2^4 < 17 ≤ 2^5)
    // Works for N≥1.
    template <int N, bool done = (N <= 1)>
    struct clog2_helper
    {
        static constexpr int value = 1 + clog2_helper<(N >> 1)>::value;
    };
    template <int N>
    struct clog2_helper<N, true>
    {
        static constexpr int value = 0;
    };

    template <int N>
    struct clog2
    {
        static constexpr int value = clog2_helper<N>::value;
    };

} // namespace detail

template <int nbits, int es>
class Posit
{
public:
    // static constexpr int max_k_size = nbits / 2;
    // static constexpr int k_bit_size = detail::clog2<nbits - 1>::value;
    static constexpr int frac_bit_size = nbits - 3 - es + 1;
    static constexpr int counter_bit_size = detail::clog2<nbits>::value + 1;
    // static constexpr int max_count_size = nbits / 2;
    static constexpr int max_exp_val = 1 << es; // pow(2, es),  max value for exponent

    struct posit_unpacked
    {
        bool sign;
        // the max amount of bits for r is nbits-1 bits, nbits-2 bits beeing 0 (or 1), and the last beeing 1 (or 0)
        // k is the amount of counted bits
        // which can be stored in log2(nbits - 2) bits
        ap_int<counter_bit_size> k;
        ap_uint<es> exp;
        // the max amount of bits for frac is nbits - 1 (sign) - 2 (min bits for k) - es;
        ap_ufixed<frac_bit_size, 1> frac;
        bool is_zero;
        bool is_inf;
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
        bits_ = other.bits_;
        /*
        if (this != &other)
        {
            bits_ = other.bits_;
        }
        */
        return *this;
    }

    Posit(int c)
    {
        // #pragma HLS INLINE

        bool psign = c >= 0 ? 0 : 1;

        // mantissa is just zeros
        ap_ufixed<2, 1> pmantissa;

        if (c == 0)
            pmantissa[1] = 0.0;
        else
            pmantissa[1] = 1.0;

        int fexponent = hls::log2(c);

        int pk, pexp;
        getKEFromExp(fexponent, pk, pexp);

        posit_unpacked unpacked;
        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.exp = pexp;
        unpacked.k = pk;

        bits_ = encode(unpacked);
    }

    Posit(unsigned int c)
    {
        // #pragma HLS INLINE
        bool psign = 0;

        // mantissa is just zeros
        ap_ufixed<2, 1> pmantissa;

        if (c == 0)
            pmantissa[1] = 0.0;
        else
            pmantissa[1] = 1.0;

        int fexponent = hls::log2(c);

        int pk, pexp;
        getKEFromExp(fexponent, pk, pexp);

        posit_unpacked unpacked;
        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.exp = pexp;
        unpacked.k = pk;

        bits_ = encode(unpacked);
    }

    Posit(float c)
    {
        // #pragma HLS INLINE

        ap_uint<32> bits = *reinterpret_cast<ap_uint<32> *>(&c);
        // unsigned int* bitsPtr = (unsigned int*)&c;
        // unsigned int bits = *bitsPtr; // Dereference to get the raw bits

        bool fsign = bits[31];
        // remove bias from floating point exponent
        // ap_int<9> fexponent = bits(30, 23) - 127;
        int fexponent = bits(30, 23) - 127;
        // get mantisa from floating point
        ap_ufixed<24, 1> fmantissa;

        if (c == 0.0f)
            fmantissa[23] = 0;
        else
            fmantissa[23] = 1;

        fmantissa(22, 0) = bits(22, 0);

        // get sign from float sign
        bool psign = fsign;

        ap_ufixed<frac_bit_size + 1, 2> pmantissa = fmantissa;

        // round to nearest
        if (fmantissa[22 - (frac_bit_size - 1)] == 1)
        {
            ap_ufixed<frac_bit_size, 1> one = 0;
            one[0] = 1;
            // if(pmantissa[0] == 0)
            pmantissa += one;
            // pmantissa[0] = 1;
        }

        // normalize (necesary because of the rounding)
        if (pmantissa >= 2.0)
        {
            pmantissa = pmantissa >> 1;
            fexponent++;
        }

        int pk, pexp;
        getKEFromExp(fexponent, pk, pexp);

        posit_unpacked unpacked;
        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.exp = pexp;
        unpacked.k = pk;

        bits_ = encode(unpacked);
    }

    Posit(double c)
    {
        // #pragma HLS INLINE

        ap_uint<64> bits = *reinterpret_cast<ap_uint<64> *>(&c);
        // unsigned int* bitsPtr = (unsigned int*)&c;
        // unsigned int bits = *bitsPtr; // Dereference to get the raw bits

        bool is_zero;
        bool is_inf;

        bool fsign;
        // ap_int<12> fexponent;
        int fexponent;
        // get mantisa from floating point
        ap_ufixed<53, 1> fmantissa;

        if (c == 0.0)
        {
            is_zero = 1;
            is_inf = 0;
            fsign = 0;
            fexponent = 0;
            fmantissa = 0;
        }
        else if (c == std::numeric_limits<double>::infinity() || c == -std::numeric_limits<double>::infinity())
        {
            is_zero = 0;
            is_inf = 1;
            fsign = 0;
            fexponent = 0;
            fmantissa = 0;
        }
        else
        {
            is_zero = 0;
            is_inf = 0;
            fsign = bits[63];
            fexponent = bits(62, 52) - 1023; // remove bias from floating point exponent
            fmantissa[52] = 1;
            fmantissa(51, 0) = bits(51, 0);
        }

        // get sign from double sign
        bool psign = fsign;

        // get mantissa from double mantissa
        ap_ufixed<frac_bit_size + 1, 2> pmantissa = fmantissa;

        // round to nearest
        if (fmantissa[51 - (frac_bit_size - 1)] == 1)
        {
            ap_ufixed<frac_bit_size, 1> one = 0;
            one[0] = 1;
            // if(pmantissa[0] == 0)
            pmantissa += one;
            // pmantissa[0] = 1;
        }

        // normalize (necesary because of the rounding)
        if (pmantissa >= 2.0)
        {
            pmantissa = pmantissa >> 1;
            fexponent++;
        }

        int pk, pexp;
        getKEFromExp(fexponent, pk, pexp);

        posit_unpacked unpacked;

        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.exp = pexp;
        unpacked.k = pk;
        unpacked.is_zero = is_zero;
        unpacked.is_inf = is_inf;

        bits_ = encode(unpacked);
    }

    template <int fnbits, int fibits>
    Posit(ap_fixed<fnbits, fibits> c)
    {
        // #pragma HLS INLINE

        bool fsign = c >= 0 ? 0 : 1;
        int fexponent = hls::log2(c(fnbits - 1, fnbits - fibits));

        ap_ufixed<fnbits - fibits + 1, 1> fmantissa;

        if (c == 0.0f)
            fmantissa[fnbits - fibits] = 0;
        else
            fmantissa[fnbits - fibits] = 1;

        fmantissa(fnbits - fibits - 1, 0) = c(fnbits - fibits - 1, 0);

        // get sign from float sign
        bool psign = fsign;

        ap_ufixed<frac_bit_size + 1, 2> pmantissa = fmantissa;

        int pk, pexp;
        getKEFromExp(fexponent, pk, pexp);

        posit_unpacked unpacked;
        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.exp = pexp;
        unpacked.k = pk;

        bits_ = encode(unpacked);
    }

    operator int() const
    {
        // #pragma HLS INLINE

        posit_unpacked unpacked = decode(bits_);

        if (unpacked.frac == 0.0)
        {
            return 0;
        }
        else
        {
            int exp = getExpFromKE(unpacked.k, unpacked.exp);
            int res = (1 << exp);
            if (unpacked.sign)
            {
                res = -res;
            }
            return res;
        }
    }

    operator float() const
    {
        // #pragma HLS INLINE

        posit_unpacked unpacked = decode(bits_);

        ap_uint<32> bits = 0;

        bits(31, 31) = unpacked.sign;

        ap_uint<8> exponent;

        if (unpacked.frac[frac_bit_size - 1] == 0)
            exponent = 0;
        else
            exponent = getExpFromKE(unpacked.k, unpacked.exp) + 127;

        bits(30, 23) = exponent;

        if (23 >= frac_bit_size - 1)
            bits(22, 22 - frac_bit_size + 2) = unpacked.frac(frac_bit_size - 2, 0);
        else
            bits(22, 0) = unpacked.frac(frac_bit_size - 2, frac_bit_size - 2 - 22);

        float fresult = *reinterpret_cast<float *>(&bits);
        return fresult;
    }

    operator double() const
    {
        // #pragma HLS INLINE

        posit_unpacked unpacked = decode(bits_);

        ap_uint<64> bits;

        if (unpacked.is_zero)
        {
            return double(0.0);
        }

        if (unpacked.is_inf)
        {
            return std::numeric_limits<double>::infinity();
        }

        bits[63] = unpacked.sign;

        ap_uint<11> exponent;

        if (unpacked.frac[frac_bit_size - 1] == 0)
            exponent = 0;
        else
            exponent = getExpFromKE(unpacked.k, unpacked.exp) + 1023;

        bits(62, 52) = exponent;

        if (52 >= frac_bit_size - 1)
        {
            bits(51, 51 - frac_bit_size + 2) = unpacked.frac(frac_bit_size - 2, 0);
            bits(51 - frac_bit_size + 1, 0) = 0; // fill the rest with zeros
        }
        else
        {
            bits(51, 0) = unpacked.frac(frac_bit_size - 2, frac_bit_size - 2 - 51);
        }

        double fresult = *reinterpret_cast<double *>(&bits);
        return fresult;
    }

    template <int fnbits, int fibits>
    operator ap_fixed<fnbits, fibits>() const
    {
        // #pragma HLS INLINE

        posit_unpacked unpacked = decode(bits_);

        if (unpacked.frac == 0.0)
        {
            return 0;
        }
        else
        {
            int exp = getExpFromKE(unpacked.k, unpacked.exp);
            int res = (unpacked.frac << exp);
            if (unpacked.sign)
            {
                res = -res;
            }
            return res;
        }
    }

    Posit fabs() const
    {
        Posit result = *this;
        if (result.bits_[nbits - 1] == 1)
        {
            result.bits_[nbits - 1] = 0; // set sign bit to 0
        }

        return result;
    }

    Posit floor() const
    {
        // floor to nearest
        posit_unpacked unpacked = decode();
        int exp = getExpFromKE(unpacked.k, unpacked.exp);

        ap_fixed<frac_bit_size * 2, frac_bit_size> frac = unpacked.frac;
        frac = frac << exp;
        frac = hls::floor(frac);
        frac = frac >> exp;

        unpacked.frac = frac;

        Posit result;
        result.bits_ = encode(unpacked);

        return result;
    }

    Posit round() const
    {
        // round to nearest
        posit_unpacked unpacked = decode(bits_);
        int exp = getExpFromKE(unpacked.k, unpacked.exp);

        ap_fixed<frac_bit_size * 2, frac_bit_size> frac = unpacked.frac;
        frac = frac << exp;
        frac = hls::round(frac);
        frac = frac >> exp;

        unpacked.frac = frac;

        Posit result;
        result.encode(unpacked);

        return result;
    }

    Posit ceil() const
    {
        // round to nearest
        posit_unpacked unpacked = decode();
        int exp = getExpFromKE(unpacked.k, unpacked.exp);

        ap_fixed<frac_bit_size * 2, frac_bit_size> frac = unpacked.frac;
        frac = frac << exp;
        frac = hls::ceil(frac);
        frac = frac >> exp;

        unpacked.frac = frac;

        Posit result;
        result.bits_ = encode(unpacked);

        return result;
    }

    Posit operator-() const
    {
        Posit result = *this;
        result.bits_[nbits - 1] = !result.bits_[nbits - 1];
        return result;
    }

    Posit &operator+=(const Posit &other)
    {
        *this = *this + other;
        return *this;
    }

    Posit &operator-=(const Posit &other)
    {
        *this = *this - other;
        return *this;
    }

    Posit &operator*=(const Posit &other)
    {
        *this = *this * other;
        return *this;
    }

    Posit &operator/=(const Posit &other)
    {
        *this = *this / other;
        return *this;
    }

    bool operator<(const Posit &rhs) const
    {
        posit_unpacked unpacked_1 = decode(bits_);
        posit_unpacked unpacked_2 = rhs.decode(rhs.bits_);

        if (unpacked_1.sign == unpacked_2.sign)
        {
            if (unpacked_1.k == unpacked_2.k)
            {
                if (unpacked_1.exp == unpacked_2.exp)
                {
                    if (unpacked_1.frac == unpacked_2.frac)
                    {
                        return false;
                    }
                    else
                    {

                        return unpacked_1.sign != (unpacked_1.frac < unpacked_2.frac);
                    }
                }
                else
                {
                    return unpacked_1.sign != (unpacked_1.exp < unpacked_2.exp);
                }
            }
            else
            {
                return unpacked_1.sign != (unpacked_1.k < unpacked_2.k);
            }
        }
        else
        {
            return unpacked_1.sign;
        }
    }

    bool operator>(const Posit &rhs) const
    {
        posit_unpacked unpacked_1 = decode(bits_);
        posit_unpacked unpacked_2 = rhs.decode(rhs.bits_);

        if (unpacked_1.sign == unpacked_2.sign)
        {
            if (unpacked_1.k == unpacked_2.k)
            {
                if (unpacked_1.exp == unpacked_2.exp)
                {
                    if (unpacked_1.frac == unpacked_2.frac)
                    {
                        return false;
                    }
                    else
                    {

                        return unpacked_1.sign != (unpacked_1.frac > unpacked_2.frac);
                    }
                }
                else
                {
                    return unpacked_1.sign != (unpacked_1.exp > unpacked_2.exp);
                }
            }
            else
            {
                return unpacked_1.sign != (unpacked_1.k > unpacked_2.k);
            }
        }
        else
        {
            return !unpacked_1.sign;
        }
    }

    bool operator<=(const Posit &rhs) const
    {
        posit_unpacked unpacked_1 = decode(bits_);
        posit_unpacked unpacked_2 = rhs.decode(rhs.bits_);

        if (unpacked_1.sign == unpacked_2.sign)
        {
            if (unpacked_1.k == unpacked_2.k)
            {
                if (unpacked_1.exp == unpacked_2.exp)
                {
                    if (unpacked_1.frac == unpacked_2.frac)
                    {
                        return true;
                    }
                    else
                    {

                        return unpacked_1.sign != (unpacked_1.frac <= unpacked_2.frac);
                    }
                }
                else
                {
                    return unpacked_1.sign != (unpacked_1.exp <= unpacked_2.exp);
                }
            }
            else
            {
                return unpacked_1.sign != (unpacked_1.k <= unpacked_2.k);
            }
        }
        else
        {
            return unpacked_1.sign;
        }
    }

    bool operator>=(const Posit &rhs) const
    {
        posit_unpacked unpacked_1 = decode(bits_);
        posit_unpacked unpacked_2 = rhs.decode(rhs.bits_);

        if (unpacked_1.sign == unpacked_2.sign)
        {
            if (unpacked_1.k == unpacked_2.k)
            {
                if (unpacked_1.exp == unpacked_2.exp)
                {
                    if (unpacked_1.frac == unpacked_2.frac)
                    {
                        return true;
                    }
                    else
                    {

                        return unpacked_1.sign != (unpacked_1.frac >= unpacked_2.frac);
                    }
                }
                else
                {
                    return unpacked_1.sign != (unpacked_1.exp >= unpacked_2.exp);
                }
            }
            else
            {
                return unpacked_1.sign != (unpacked_1.k >= unpacked_2.k);
            }
        }
        else
        {
            return !unpacked_1.sign;
        }
    }

    bool operator==(const Posit &rhs) const
    {
        if (bits_ == rhs.bits_)
            return true;
        else
            return false;
    }

    bool operator!=(const Posit &rhs) const
    {
        if (bits_ != rhs.bits_)
            return true;
        else
            return false;
    }

    Posit operator+(const Posit &rhs) const
    {
        posit_unpacked in1 = decode(bits_);
        posit_unpacked in2 = rhs.decode(rhs.bits_);

        bool is_inf = in1.is_inf || in2.is_inf;

        int exp1 = getExpFromKE(in1.k, in1.exp);
        int exp2 = getExpFromKE(in2.k, in2.exp);

        // set biggest posit to be in1
        int diff_texp = exp1 - exp2;

        if (diff_texp < 0)
        {
            posit_unpacked paux = in1;
            in1 = in2;
            in2 = paux;

            exp1 = exp2;
            diff_texp = -diff_texp;
        }

        // add the sign back into the fraction, so that we can do the sum
        ap_fixed<frac_bit_size + 2, 2> frac1 = in1.frac;
        ap_fixed<frac_bit_size + 2, 2> frac2 = in2.frac;

        if (in1.sign)
            frac1 = -frac1;
        else
            frac1 = frac1;

        if (in2.sign)
            frac2 = -frac2;
        else
            frac2 = frac2;

        // shift left in2, so that both have the same exponent
        // for (int i = 0; i < diff_texp; i++)
        //    frac2 = frac2 >> 1;
        frac2 = frac2 >> diff_texp;

        // do addition
        ap_fixed<frac_bit_size + 4, 3> frac = frac1 + frac2;
        int exp = exp1;

        bool is_zero;
        if (!is_inf && frac == 0)
            is_zero = 1;
        else
            is_zero = 0;

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
        if (frac > 0)
        {
        Posit_add_while_1:
            while (frac >= 2)
            {
                frac = frac >> 1;
                exp++;
            }

        Posit_add_while_2:
            while (frac < 1)
            {
                frac = frac << 1;
                exp--;
            }
        }

        // round to nearest
        ap_ufixed<frac_bit_size + 1, 2> rfrac = frac;
        if (frac[frac_bit_size + 4 - 4 - (frac_bit_size - 1)] == 1)
        {
            ap_ufixed<frac_bit_size, 1> one = 0;
            one[0] = 1;
            // if(pmantissa[0] == 0)
            rfrac += one;
            // pmantissa[0] = 1;
        }

        // normalize fraction (again)
        if (rfrac >= 2)
        {
            rfrac = rfrac >> 1;
            exp++;
        }

        int k, e;
        getKEFromExp(exp, k, e);

        posit_unpacked out;
        out.sign = sign;
        out.k = k;
        out.exp = e;
        out.frac = rfrac;
        out.is_inf = 0;
        out.is_zero = 0;

        Posit output;
        output.bits_ = encode(out);

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
        posit_unpacked in1 = decode(bits_);
        posit_unpacked in2 = rhs.decode(rhs.bits_);

        bool sign;
        ap_int<counter_bit_size + 2> k;
        ap_uint<es + 2> exp;
        ap_ufixed<frac_bit_size * 2, 2> frac;

        bool is_inf = in1.is_inf || in2.is_inf;
        bool is_zero;
        if (is_inf)
        {
            is_zero = 0;
            sign = 0;
            k = 0;
            exp = 0;
            frac = 0;
        }
        else if (in1.is_zero || in2.is_zero)
        {
            is_zero = 1;
            sign = 0;
            k = 0;
            exp = 0;
            frac = 0;
        }
        else
        {
            is_zero = 0;
            sign = in1.sign ^ in2.sign;
            k = in1.k + in2.k;
            exp = in1.exp + in2.exp;
            frac = in1.frac * in2.frac;
        }

        // normalize fraction
        if (frac >= 2)
        {
            frac = frac >> 1;
            exp++;
        }

        // normalize exponent
        if (exp >= max_exp_val)
        {
            exp -= max_exp_val;
            k++;
        }

        // round to nearest
        ap_ufixed<frac_bit_size + 1, 2> rfrac = frac;
        if (frac[frac_bit_size * 2 - 3 - (frac_bit_size - 1)] == 1)
        {
            ap_ufixed<frac_bit_size, 1> one = 0;
            one[0] = 1;
            // if(pmantissa[0] == 0)
            rfrac += one;
            // pmantissa[0] = 1;
        }

        // normalize fraction (again)
        if (rfrac >= 2)
        {
            rfrac = rfrac >> 1;
            exp++;
        }

        // normalize exponent (again)
        if (exp >= max_exp_val)
        {
            exp -= max_exp_val;
            k++;
        }

        posit_unpacked out;
        out.sign = sign;
        out.k = k;
        out.exp = exp;
        out.frac = rfrac;
        out.is_zero = is_zero;
        out.is_inf = is_inf;

        Posit output;
        output.bits_ = output.encode(out);

        return output;
    }

    Posit operator/(const Posit &rhs) const
    {
        posit_unpacked in1 = decode(bits_);
        posit_unpacked in2 = rhs.decode(rhs.bits_);

        bool is_inf = in1.is_inf || in2.is_inf;

        bool sign;
        ap_int<counter_bit_size + 2> k;
        ap_int<es + 2> exp;
        ap_ufixed<frac_bit_size * 2, 2> frac;

        if (is_inf)
        {
            is_zero = 0;
            sign = 0;
            k = 0;
            exp = 0;
            frac = 0;
        }
        else if (in2.is_zero == 1)
        {
            is_inf = 1;
            is_zero = 0;
            sign = 0;
            k = 0;
            exp = 0;
            frac = 0;
        }
        else if(in1.is_zero == 1)
        {
            is_zero = 1;
            sign = 0;
            k = 0;
            exp = 0;
            frac = 0;
        }
        else
        {
            sign = in1.sign ^ in2.sign;
            k = in1.k - in2.k;
            in1.exp - in2.exp;
            frac = in1.frac / in2.frac;
        }

        // normalize fraction
        if (frac < 1.0)
        {
            frac = frac << 1;
            exp--;
        }

        // normalize exponent
        if (exp < 0)
        {
            exp += max_exp_val;
            k--;
        }

        /*
        if (exp < 0)
        {
            exp += hls::pow(2, es);
            k--;
        }
        */

        // round to nearest
        ap_ufixed<frac_bit_size + 1, 2> rfrac = frac;
        if (frac[frac_bit_size * 2 - 3 - (frac_bit_size - 1)] == 1)
        {
            ap_ufixed<frac_bit_size, 1> one = 0;
            one[0] = 1;
            // if(pmantissa[0] == 0)
            rfrac += one;
            // pmantissa[0] = 1;
        }

        // normalize fraction (again)
        if (rfrac >= 2)
        {
            rfrac = rfrac >> 1;
            exp++;
        }

        // normalize exponent (again)
        if (exp >= max_exp_val)
        {
            exp -= max_exp_val;
            k++;
        }

        posit_unpacked out;
        out.sign = sign;
        out.k = k;
        out.exp = exp;
        out.frac = rfrac;
        out.is_zero = 0;
        out.is_inf = is_inf;

        Posit output;
        output.bits_ = output.encode(out);

        return output;
    }

private:
    int getExpFromKE(int k, int e) const
    {
        // #pragma HLS INLINE
        // get exponent from k and e
        return k * max_exp_val + e;
    }

    void getKEFromExp(int in_exp, int &out_k, int &out_e) const
    {
        // get k and e from floating point exponent
        // How many times each exponent over - or under - flowed the valid interval
        out_k = in_exp / max_exp_val; // works for negatives too

        // New exponent in the allowed range (0, max_exp_val - 1)
        out_e = in_exp % max_exp_val;

        // If `a` and `b` have opposite signs and the remainder is non-zero,
        // the truncated result is too large; subtract one to get the floor.
        if (out_e < 0)
            --out_k;
    }

    posit_unpacked decode(const ap_uint<nbits> &bits) const
    {
        posit_unpacked unpacked;

        bool simbol = bits[nbits - 2];
        ap_uint<counter_bit_size> k;

        ap_uint<counter_bit_size> counter = 0;

        unpacked.sign = bits[nbits - 1];
        unpacked.k = 0;
        unpacked.exp = 0;
        unpacked.frac = 0.0;
        unpacked.is_zero = !bits[nbits - 1];
        unpacked.is_inf = bits[nbits - 1];

        enum states
        {
            K,
            E,
            F
        } state = K;

    Posit_decode_for:
        for (int bit = nbits - 3; bit >= 0; bit--)
        {
            if (state == K)
            {
                if (bits[bit] != simbol)
                {
                    k = nbits - 2 - bit;
                    // counter = nbits - 3 - bit;

                    if (simbol == 0)
                        unpacked.k = k - 1;
                    else
                        unpacked.k = -k;
                    counter = 0;
                    unpacked.frac[frac_bit_size - 1] = 1;
                    state = E;
                }
                continue;
            }

            if (state == E)
            {
                // unpacked.exp[es - 1 - counter] = bits[bit];
                unpacked.exp[bit - (nbits - 2 - k - es)] = bits[bit];

                counter++;
                if (counter >= es)
                {
                    state = F;
                    counter = 0;
                }
                continue;
            }

            if (state == F)
            {
                unpacked.frac[frac_bit_size - 2 - counter] = bits[bit];
                counter++;
                continue;
            }
        }

        unpacked.is_inf = 0;
        unpacked.is_zero = 0;

        return unpacked;

        /*
        posit_unpacked unpacked;

        bool simbol = 0;
        ap_uint<counter_bit_size> counter = 0;

        unpacked.k = 0;
        unpacked.exp = 0;
        unpacked.frac = 0.0;

        enum states
        {
            S,
            K,
            E,
            F
        } state = S;

    Posit_decode_for:
        for (int bit = nbits - 1; bit >= 0; bit--)
        {
            switch (state)
            {
            case S:
                unpacked.sign = bits[bit];
                state = K;
                break;

            case K:
                if (bit == nbits - 2)
                {
                    simbol = bits[bit];
                    counter = 1;
                }
                else
                {
                    if (bits[bit] == simbol)
                    {
                        counter++;
                    }
                    else
                    {
                        if (simbol == 0)
                            unpacked.k = counter - 1;
                        else
                            unpacked.k = -counter;
                        counter = 0;
                        unpacked.frac[frac_bit_size - 1] = 1;
                        state = E;
                    }
                }
                break;

            case E:
                unpacked.exp[es - 1 - counter] = bits[bit];
                counter++;
                if (counter >= es)
                {
                    state = F;
                    counter = 0;
                }
                break;

            case F:
                unpacked.frac[frac_bit_size - 2 - counter] = bits[bit];
                counter++;
                break;

            default:
                break;
            }
        }

        return unpacked;
        */
        /*
        bool simbol = bits[nbits - 2];
        ap_uint<counter_bit_size> k = 1;

        // bool s = bits[nbits - 1];
        // ap_uint<es> e = 0;
        // ap_fixed<frac_bit_size, 1> f = 1.0;

        posit_unpacked unpacked;

        unpacked.sign = bits[nbits - 1];
        unpacked.k = 1;
        unpacked.exp = 0;
        unpacked.frac = 1.0;

    Posit_decode_for:
        for (int i = 0; i < nbits - 2; i++)
        {
            if (bits[nbits - 3 - i] == simbol)
            {
                k++;
            }
            else
            {
                break;
            }

            //if (bits[nbits - 3 - i] != simbol)
            //{
            //    k = i + 1;
            //    break;
            //}
        }

        if (k == nbits - 1)
            unpacked.k = 0;
        else
        {
            if (simbol == 0)
                unpacked.k = k - 1;
            else
                unpacked.k = -k;
        }

        // exponent bits
        ap_int<counter_bit_size + 1> exp_start = nbits - 3 - k;
        ap_int<counter_bit_size + 1> exp_end = exp_start - es + 1;
        if (exp_end < 0)
            exp_end = 0;
        ap_int<counter_bit_size> exp_len = exp_start - exp_end + 1;

        if (exp_len > 0)
            unpacked.exp(exp_len - 1, 0) = bits_(exp_start, exp_end);
        // else
        //     unpacked.exp = 0;

        // fraction bits
        ap_int<counter_bit_size + 1> frac_start = exp_end - 1;
        ap_int<counter_bit_size + 1> frac_len = frac_start + 1;

        // add leading 1
        if (k == nbits - 1)
            unpacked.frac[frac_bit_size - 1] = 0;
        else
            unpacked.frac[frac_bit_size - 1] = 1;

        // if there is more bits to read for the fraction, read them
        if (frac_len > 0)
            unpacked.frac(frac_bit_size - 2, frac_bit_size - 2 - frac_len + 1) = bits_(frac_start, 0);
        else
            unpacked.frac(frac_bit_size - 2, 0) = 0;

        return unpacked;
        */
    }

    ap_uint<nbits> encode(const posit_unpacked &unpacked) const
    {
        ap_uint<nbits> bits;

        // k bits
        bool reg_bit;
        int reg_len;

        // if the integer bit in the fraction is 0, then the whole number is zero
        if (unpacked.is_zero == 1)
        {
            bits[nbits - 1] = 0;
            reg_bit = 0;
            reg_len = nbits - 1;
        }
        else if (unpacked.is_inf == 1)
        {
            bits[nbits - 1] = 1;
            reg_bit = 0;
            reg_len = nbits - 1;
        }
        else
        {
            bits[nbits - 1] = unpacked.sign;
            reg_bit = unpacked.k >= 0 ? 0 : 1;
            reg_len = unpacked.k >= 0 ? int(unpacked.k + 1) : int(-unpacked.k);
            reg_len = hls::min(nbits - 1, reg_len);
        }

        int reg_start = nbits - 2;

    Posit_encode_for:
        for (int i = 0; i < reg_len; i++)
        {
            bits[reg_start - i] = reg_bit;
        }

        int reg_end = reg_start - reg_len;

        if (reg_end >= 0)
        {
            bits[reg_end] = !reg_bit;
        }

        // exponent bits
        int exp_start = nbits - 3 - reg_len;
        int exp_end = hls::max(exp_start - es + 1, 0);
        int exp_len = exp_start - exp_end + 1;

        if (exp_len > 0)
            bits(exp_start, exp_end) = unpacked.exp; // unpacked.exp(exp_len - 1, 0);

        // fraction bits
        int frac_start = exp_end - 1;
        int frac_end = 0;
        int frac_len = frac_start - frac_end + 1;

        if (frac_len > 0)
            bits(frac_start, frac_end) = unpacked.frac(frac_bit_size - 2, frac_bit_size - 1 - frac_len);

        return bits;
    }

    ap_uint<nbits> bits_;
};

template <int nbits, int es>
Posit<nbits, es> fabs(const Posit<nbits, es> &p)
{
    return p.fabs();
}

template <int nbits, int es>
Posit<nbits, es> floor(const Posit<nbits, es> &p)
{
    return p.floor();
}

template <int nbits, int es>
Posit<nbits, es> round(const Posit<nbits, es> &p)
{
    return p.round();
}

template <int nbits, int es>
Posit<nbits, es> ceil(const Posit<nbits, es> &p)
{
    return p.ceil();
}