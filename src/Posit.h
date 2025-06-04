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
    static constexpr int max_k_size = nbits - 1;
    // static constexpr int max_k_size = detail::clog2<nbits - 1>::value;
    static constexpr int max_frac_size = nbits - 3 - es + 1;
    static constexpr int max_exp_val = 2 << (es - 1); // pow(2, es),  max value for exponent
    static constexpr int float_exp_bias = 127;        // pow(2, 7) - 1;
    static constexpr int double_exp_bias = 1023;      // pow(2, 10) -1;

    struct posit_unpacked
    {
        bool sign;
        // the max amount of bits for r is nbits-1 bits, nbits-2 bits beeing 0 (or 1), and the last beeing 1 (or 0)
        // k is the amount of counted bits
        // which can be stored in log2(nbits - 2) bits
        ap_int<max_k_size> k;
        ap_uint<es> exp;
        // the max amount of bits for frac is nbits - 1 (sign) - 2 (min bits for k) - es;
        ap_ufixed<max_frac_size, 1> frac;
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
        
        if(c == 0)
            pmantissa[1] = 0.0;
        else
            pmantissa[1] = 1.0;

        ap_int<9> fexponent = hls::log2(c);

        // get k and e from floating point exponent
        // How many times each exponent over - or under - flowed the valid interval
        ap_int<max_k_size> pk = fexponent / max_exp_val; // works for negatives too

        // New exponent in the allowed range (0, max_exp_val - 1)
        ap_int<9> pexponent = fexponent % max_exp_val;

        // If `a` and `b` have opposite signs and the remainder is non-zero,
        // the truncated result is too large; subtract one to get the floor.
        if (pexponent < 0)
            --pk;

        posit_unpacked unpacked;
        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.exp = pexponent;
        unpacked.k = pk;

        encode(unpacked);
    }

    Posit(unsigned int c)
    {
        // #pragma HLS INLINE
        *this = Posit(int(c));
    }

    Posit(float c)
    {
        // #pragma HLS INLINE

        ap_uint<32> bits = *reinterpret_cast<ap_uint<32> *>(&c);
        // unsigned int* bitsPtr = (unsigned int*)&c;
        // unsigned int bits = *bitsPtr; // Dereference to get the raw bits

        bool fsign = bits[31];
        // remove bias from floating point exponent
        ap_int<9> fexponent = bits(30, 23) - float_exp_bias;
        // get mantisa from floating point
        ap_ufixed<24, 1> fmantissa;
        
        if(c == 0.0f)
            fmantissa[23] = 0;
        else
            fmantissa[23] = 1;
        
        fmantissa(22, 0) = bits(22, 0);

        // get sign from float sign
        bool psign = fsign;

        ap_ufixed<max_frac_size + 1, 2> pmantissa = fmantissa;

        // round to nearest
        if (fmantissa[22 - (max_frac_size - 1)] == 1)
        {
            ap_ufixed<max_frac_size, 1> one = 0;
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

        // get k and e from floating point exponent
        // How many times each exponent over - or under - flowed the valid interval
        ap_int<max_k_size> pk = fexponent / max_exp_val; // works for negatives too

        // New exponent in the allowed range (0, max_exp_val - 1)
        ap_int<9> pexponent = fexponent % max_exp_val;

        // If `a` and `b` have opposite signs and the remainder is non-zero,
        // the truncated result is too large; subtract one to get the floor.
        if (pexponent < 0)
            --pk;

        posit_unpacked unpacked;
        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.exp = pexponent;
        unpacked.k = pk;

        encode(unpacked);
    }

    Posit(double c)
    {
        // #pragma HLS INLINE

        ap_uint<64> bits = *reinterpret_cast<ap_uint<64> *>(&c);
        // unsigned int* bitsPtr = (unsigned int*)&c;
        // unsigned int bits = *bitsPtr; // Dereference to get the raw bits

        bool fsign = bits[63];
        // remove bias from floating point exponent
        ap_int<12> fexponent = bits(62, 52) - double_exp_bias;
        // get mantisa from floating point
        ap_ufixed<53, 1> fmantissa;
        
        if(c == 0.0)
            fmantissa[52] = 0;
        else
            fmantissa[52] = 1;

        fmantissa(51, 0) = bits(51, 0);

        // get sign from double sign
        bool psign = fsign;

        // get mantissa from double mantissa
        ap_ufixed<max_frac_size + 1, 2> pmantissa = fmantissa;

        // round to nearest
        if (fmantissa[51 - (max_frac_size - 1)] == 1)
        {
            ap_ufixed<max_frac_size, 1> one = 0;
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

        // get k and e from floating point exponent
        // How many times each exponent over - or under - flowed the valid interval
        ap_int<max_k_size> pk = fexponent / max_exp_val; // works for negatives too

        // New exponent in the allowed range (0, max_exp_val - 1)
        ap_int<9> pexponent = fexponent % max_exp_val;

        // If `a` and `b` have opposite signs and the remainder is non-zero,
        // the truncated result is too large; subtract one to get the floor.
        if (pexponent < 0)
            --pk;

        posit_unpacked unpacked;
        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.exp = pexponent;
        unpacked.k = pk;

        encode(unpacked);
    }

    operator float() const
    {
        // #pragma HLS INLINE

        posit_unpacked unpacked = decode();

        ap_uint<32> bits = 0;

        bits(31, 31) = unpacked.sign;

        ap_uint<8> exponent;

        if (unpacked.frac[max_frac_size - 1] == 0)
            exponent = 0;
        else
            exponent = unpacked.exp + unpacked.k * max_exp_val + float_exp_bias;

        bits(30, 23) = exponent;

        if (23 >= max_frac_size - 1)
            bits(22, 22 - max_frac_size + 2) = unpacked.frac(max_frac_size - 2, 0);
        else
            bits(22, 0) = unpacked.frac(max_frac_size - 2, max_frac_size - 2 - 22);

        float fresult = *reinterpret_cast<float *>(&bits);
        return fresult;
    }

    operator double() const
    {
        // #pragma HLS INLINE

        posit_unpacked unpacked = decode();

        ap_uint<64> bits = 0;

        bits[63] = unpacked.sign;

        ap_uint<11> exponent;

        if (unpacked.frac[max_frac_size - 1] == 0)
            exponent = 0;
        else
            exponent = unpacked.exp + unpacked.k * max_exp_val + double_exp_bias;

        bits(62, 52) = exponent;

        if (52 >= max_frac_size - 1)
            bits(51, 51 - max_frac_size + 2) = unpacked.frac(max_frac_size - 2, 0);
        else
            bits(51, 0) = unpacked.frac(max_frac_size - 2, max_frac_size - 2 - 51);

        double fresult = *reinterpret_cast<double *>(&bits);
        return fresult;
    }

    operator int() const
    {
        // #pragma HLS INLINE

        posit_unpacked unpacked = decode();

        if (unpacked.frac == 0.0)
        {
            return 0;
        }
        else
        {
            int exponent = unpacked.k * max_exp_val /*hls::pow(2, es)*/ + unpacked.exp;
            int res = (2 << exponent) /*hls::pow(2, exponent)*/;
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
        ap_int<max_k_size + 1> exp = unpacked.k * max_exp_val + unpacked.exp;

        ap_fixed<max_frac_size * 2, max_frac_size> frac = unpacked.frac;
        frac = frac << exp;
        frac = hls::floor(frac);
        frac = frac >> exp;

        unpacked.frac = frac;

        Posit result;
        result.encode(unpacked);

        return result;
    }

    Posit round() const
    {
        // round to nearest
        posit_unpacked unpacked = decode();
        ap_int<max_k_size + 1> exp = unpacked.k * max_exp_val + unpacked.exp;

        ap_fixed<max_frac_size * 2, max_frac_size> frac = unpacked.frac;
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
        ap_int<max_k_size + 1> exp = unpacked.k * max_exp_val + unpacked.exp;

        ap_fixed<max_frac_size * 2, max_frac_size> frac = unpacked.frac;
        frac = frac << exp;
        frac = hls::ceil(frac);
        frac = frac >> exp;

        unpacked.frac = frac;

        Posit result;
        result.encode(unpacked);

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
        posit_unpacked unpacked_1 = decode();
        posit_unpacked unpacked_2 = rhs.decode();

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
        posit_unpacked unpacked_1 = decode();
        posit_unpacked unpacked_2 = rhs.decode();

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
        posit_unpacked unpacked_1 = decode();
        posit_unpacked unpacked_2 = rhs.decode();

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
        posit_unpacked unpacked_1 = decode();
        posit_unpacked unpacked_2 = rhs.decode();

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
        posit_unpacked in1 = decode();
        posit_unpacked in2 = rhs.decode();

        // set biggest posit to be in1
        ap_int<max_k_size + es> diff_texp = (in1.k - in2.k) * max_exp_val + in1.exp - in2.exp;

        if (diff_texp < 0)
        {
            posit_unpacked paux = in1;
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
        //for (int i = 0; i < diff_texp; i++)
        //    frac2 = frac2 >> 1;
        frac2 = frac2 >> diff_texp;

        // do addition
        ap_fixed<max_frac_size + 4, 3> frac = frac1 + frac2;

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

        Posit_add_while_3:
            while (exp >= max_exp_val /*hls::pow(2, es)*/)
            {
                exp -= max_exp_val /*hls::pow(2, es)*/;
                k++;
            }

        Posit_add_while_4:
            while (exp < 0)
            {
                exp += max_exp_val /*hls::pow(2, es)*/;
                k--;
            }
        }

        // round to nearest
        ap_ufixed<max_frac_size + 1, 2> rfrac = frac;
        if (frac[max_frac_size + 4 - 4 - (max_frac_size - 1)] == 1)
        {
            ap_ufixed<max_frac_size, 1> one = 0;
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
        if (exp >= max_exp_val /*hls::pow(2, es)*/)
        {
            exp -= max_exp_val /*hls::pow(2, es)*/;
            k++;
        }

        posit_unpacked out;
        out.sign = sign;
        out.k = k;
        out.exp = exp;
        out.frac = rfrac;

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
        posit_unpacked in1 = decode();
        posit_unpacked in2 = rhs.decode();

        bool sign = in1.sign ^ in2.sign;
        ap_int<max_k_size + 1> k = in1.k + in2.k;
        ap_uint<es + 1> exp = in1.exp + in2.exp;
        ap_ufixed<max_frac_size * 2, 2> frac = in1.frac * in2.frac;

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

        /*
        if (exp < 0)
        {
            exp += hls::pow(2, es);
            k--;
        }
        */

        // if (frac(max_frac_size * 2 - 1, max_frac_size * 2 - 2) == 0)
        if (frac == 0)
        {
            sign = 0;
            k = 0;
            exp = 0;
            frac = 0;
        }

        // round to nearest
        ap_ufixed<max_frac_size + 1, 2> rfrac = frac;
        if (frac[max_frac_size * 2 - 3 - (max_frac_size - 1)] == 1)
        {
            ap_ufixed<max_frac_size, 1> one = 0;
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

        Posit output;
        output.encode(out);

        return output;
    }

    Posit operator/(const Posit &rhs) const
    {
        posit_unpacked in = rhs.decode();
        posit_unpacked out;
        out.sign = in.sign;
        ap_int<max_k_size> k = -in.k;
        ap_int<es + 1> exp = -in.exp;

        if (exp < 0)
        {
            exp += max_exp_val /*hls::pow(2, es)*/;
            k -= 1;
        }

        out.k = k;
        out.exp = exp;

        if (in.is_zero)
        {
            out.is_zero = false;
            out.is_inf = true;
        }
        else
        {
            out.is_zero = false;
            out.is_inf = false;
            out.frac = ap_ufixed<max_frac_size, 1>(1.0) / in.frac;
        }

        Posit inv;
        inv.encode(out);

        Posit result = (*this) * inv;

        return result;
    }

private:
    posit_unpacked decode() const
    {
        // Posit sign (1 bit), r (variable bits), e (es bits), frac (variable bits)
        // to go from a Posit into a real number
        // x = -(1 - sign) * (u**k) * (2**e) * (1, frac)
        // with u = 2**(2**es)

        posit_unpacked unpacked;

        // sign bit
        unpacked.sign = bits_[nbits - 1];

        // count identical bits
        int reg_len = 1;
        bool reg_bit = bits_[nbits - 2];
    Posit_decode_while:
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

        unpacked.exp = 0;

        if (exp_len > 0)
            unpacked.exp(exp_len - 1, 0) = bits_(exp_start, exp_end);
        // else
        //     unpacked.exp = 0;

        // fraction bits
        int frac_start = exp_end - 1;
        int frac_end = 0;
        int frac_len = frac_start - frac_end + 1;

        unpacked.frac = 0;

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

    void encode(const posit_unpacked unpacked)
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
            reg_len = hls::min(nbits - 1, reg_len);
        }

        int reg_start = nbits - 2;

    Posit_encode_for:
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