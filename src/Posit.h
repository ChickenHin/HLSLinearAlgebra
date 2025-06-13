#pragma once

#include "hls_math.h"
#include "ap_int.h"
#include "ap_fixed.h"

#include "common.h"

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

template <int kbits, int ebits, int fbits>
class posit_unpacked
{
public:
    template <int nbits, int es>
    ap_uint<nbits> encode() const
    {
        ap_uint<nbits> bits;

        // k bits
        bool reg_bit;
        int reg_len;

        // if the integer bit in the fraction is 0, then the whole number is zero
        if (is_zero == 1)
        {
            bits[nbits - 1] = 0;
            reg_bit = 0;
            reg_len = nbits - 1;
        }
        else if (is_inf == 1)
        {
            bits[nbits - 1] = 1;
            reg_bit = 0;
            reg_len = nbits - 1;
        }
        else
        {
            bits[nbits - 1] = sign;
            reg_bit = k >= 0 ? 0 : 1;
            reg_len = k >= 0 ? int(k + 1) : int(-k);
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
            bits(exp_start, exp_end) = exp; // unpacked.exp(exp_len - 1, 0);

        // fraction bits
        int frac_start = exp_end - 1;
        int frac_end = 0;
        int frac_len = frac_start - frac_end + 1;

        if (frac_len > 0)
            bits(frac_start, frac_end) = frac(fbits - 2, fbits - 1 - frac_len);

        return bits;
    }

    template <int nbits, int es>
    void decode(const ap_uint<nbits> &bits)
    {
        bool simbol = bits[nbits - 2];
        ap_uint<kbits> lenght = 0;
        ap_uint<kbits> e_counter = 0;
        ap_uint<kbits> f_counter = 0;

        // start as zero or inf depending on first bit
        is_zero = !bits[nbits - 1];
        is_inf = bits[nbits - 1];

        sign = bits[nbits - 1];
        k = 0;
        exp = 0;
        frac = 0.0;

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
                    lenght = nbits - 2 - bit;
                    // counter = nbits - 3 - bit;

                    if (simbol == 0)
                        k = lenght - 1;
                    else
                        k = -lenght;

                    state = E;
                }
                continue;
            }

            if (state == E)
            {
                // unpacked.exp[es - 1 - counter] = bits[bit];
                exp[bit - (nbits - 2 - lenght - es)] = bits[bit];

                // if we reach this state, the number is not zero nor infinite
                is_inf = 0;
                is_zero = 0;

                e_counter++;
                if (e_counter >= es)
                {
                    state = F;

                    frac[fbits - 1] = 1;
                }
                continue;
            }

            if (state == F)
            {
                frac[fbits - 2 - f_counter] = bits[bit];
                f_counter++;
                continue;
            }
        }

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

    ap_int<kbits + ebits> getTotalExp() const
    {
        // #pragma HLS INLINE
        // get exponent from k and e
        return k * (1 << ebits) + exp;
    }

    void setKEFromTotalExp(ap_int<kbits + ebits> in_exp)
    {
        // get k and e from floating point exponent
        // How many times each exponent over - or under - flowed the valid interval
        k = in_exp / (1 << ebits); // works for negatives too

        // New exponent in the allowed range (0, max_exp_val - 1)
        exp = in_exp % (1 << ebits);

        // If `a` and `b` have opposite signs and the remainder is non-zero,
        // the truncated result is too large; subtract one to get the floor.
        if (exp < 0)
        {
            --k;
            // exp = 0;
        }
    }

    bool is_zero;
    bool is_inf;
    bool sign;
    // the max amount of bits for r is nbits-1 bits, nbits-2 bits beeing 0 (or 1), and the last beeing 1 (or 0)
    // k is the amount of counted bits
    // which can be stored in log2(nbits - 2) bits
    ap_int<kbits> k;
    ap_int<ebits + 1> exp;
    // the max amount of bits for frac is nbits - 1 (sign) - 2 (min bits for k) - es;
    ap_ufixed<fbits, 1> frac;
};

template <int kbits, int ebits, int fbits>
posit_unpacked<kbits, ebits, fbits> posit_adder(const posit_unpacked<kbits, ebits, fbits> &in1, const posit_unpacked<kbits, ebits, fbits> &in2)
{
    bool is_inf = in1.is_inf || in2.is_inf;

    ap_int<kbits + ebits> exp1 = in1.getTotalExp();
    ap_int<kbits + ebits> exp2 = in2.getTotalExp();

    // set biggest posit to be in1
    ap_int<kbits + ebits> diff_texp = exp1 - exp2;

    ap_fixed<fbits + 2, 2> frac1;
    ap_fixed<fbits + 2, 2> frac2;
    ap_int<kbits + ebits> exp;

    if (diff_texp > 0)
    {
        if (!in1.sign)
            frac1 = in1.frac;
        else
            frac1 = -in1.frac;
        if (!in2.sign)
            frac2 = in2.frac;
        else
            frac2 = -in2.frac;
        exp = exp1;
    }
    else
    {
        if (!in2.sign)
            frac1 = in2.frac;
        else
            frac1 = -in2.frac;
        if (!in1.sign)
            frac2 = in1.frac;
        else
            frac2 = -in1.frac;
        exp = exp2;

        diff_texp = -diff_texp;
    }

    // shift left in2, so that both have the same exponent
    // for (int i = 0; i < diff_texp; i++)
    //    frac2 = frac2 >> 1;
    frac2 = frac2 >> diff_texp;

    // do addition
    ap_fixed<fbits + 4, 3> frac = frac1 + frac2;

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
    ap_ufixed<fbits + 1, 2> rfrac = frac;
    if (frac[fbits + 4 - 4 - (fbits - 1)] == 1)
    {
        ap_ufixed<fbits, 1> one = 0;
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

    posit_unpacked<kbits, ebits, fbits> out;

    out.setKEFromTotalExp(exp);

    out.sign = sign;
    out.frac = rfrac;
    out.is_inf = is_inf;
    out.is_zero = is_zero;

    return out;
}

template <int kbits, int ebits, int fbits>
posit_unpacked<kbits, ebits, fbits> posit_mult(const posit_unpacked<kbits, ebits, fbits> &in1, const posit_unpacked<kbits, ebits, fbits> &in2)
{
    bool sign;
    ap_int<kbits + 2> k;
    ap_uint<ebits + 2> exp;
    ap_ufixed<fbits * 2, 2> frac;

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
    if (exp >= (1 << ebits))
    {
        exp -= (1 << ebits);
        k++;
    }

    // round to nearest
    ap_ufixed<fbits + 1, 2> rfrac = frac;
    if (frac[fbits * 2 - 3 - (fbits - 1)] == 1)
    {
        ap_ufixed<fbits, 1> one = 0;
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
    if (exp >= (1 << ebits))
    {
        exp -= (1 << ebits);
        k++;
    }

    posit_unpacked<kbits, ebits, fbits> out;
    out.sign = sign;
    out.k = k;
    out.exp = exp;
    out.frac = rfrac;
    out.is_zero = is_zero;
    out.is_inf = is_inf;

    return out;
}

template <int kbits, int ebits, int fbits>
posit_unpacked<kbits, ebits, fbits> posit_div(const posit_unpacked<kbits, ebits, fbits> &in1, const posit_unpacked<kbits, ebits, fbits> &in2)
{
    bool is_inf = in1.is_inf || in2.is_inf;
    bool is_zero;
    bool sign;
    ap_int<kbits + 2> k;
    ap_int<ebits + 2> exp;
    ap_ufixed<fbits * 2, 2> frac;

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
    else if (in1.is_zero == 1)
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
        k = in1.k - in2.k;
        exp = in1.exp - in2.exp;
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
        exp += (1 << ebits);
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
    ap_ufixed<fbits + 1, 2> rfrac = frac;
    if (frac[fbits * 2 - 3 - (fbits - 1)] == 1)
    {
        ap_ufixed<fbits, 1> one = 0;
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
    if (exp >= (1 << ebits))
    {
        exp -= (1 << ebits);
        k++;
    }

    posit_unpacked<kbits, ebits, fbits> out;
    out.sign = sign;
    out.k = k;
    out.exp = exp;
    out.frac = rfrac;
    out.is_zero = is_zero;
    out.is_inf = is_inf;

    return out;
}

template <int kbits, int ebits, int fbits>
posit_unpacked<kbits, ebits, fbits> posit_mac(const posit_unpacked<kbits, ebits, fbits> &in1, const posit_unpacked<kbits, ebits, fbits> &in2, const posit_unpacked<kbits, ebits, fbits> &in3)
{
    posit_unpacked<kbits, ebits, fbits> mult_result = posit_mult(in2, in3);
    posit_unpacked<kbits, ebits, fbits> add_result = posit_adder(in1, mult_result);

    return add_result;
}

template <int kbits, int ebits, int fbits>
bool posit_equal(const posit_unpacked<kbits, ebits, fbits> &in1, const posit_unpacked<kbits, ebits, fbits> &in2)
{
    if (in1.is_zero && in2.is_zero)
    {
        return true;
    }

    if (in1.is_inf && in2.is_inf)
    {
        return true;
    }

    if (in1.is_zero || in2.is_zero)
    {
        return false;
    }

    if (in1.sign != in2.sign)
    {
        return false;
    }

    if (in1.k != in2.k)
    {
        return false;
    }

    if (in1.exp != in2.exp)
    {
        return false;
    }

    if (in1.frac != in2.frac)
    {
        return false;
    }

    return true;
}

template <int kbits, int ebits, int fbits>
bool posit_lessthan(const posit_unpacked<kbits, ebits, fbits> &in1, const posit_unpacked<kbits, ebits, fbits> &in2)
{
    if (in1.is_zero && in2.is_zero)
    {
        return false;
    }

    if (in1.is_inf && in2.is_inf)
    {
        return false;
    }

    if (in1.is_zero && !in2.is_zero)
    {
        return !in2.sign;
    }

    if (in2.is_zero && !in1.is_zero)
    {
        return in1.sign;
    }

    if (in1.sign != in2.sign)
    {
        return in1.sign;
    }

    if (in1.k != in2.k)
    {
        return in1.sign != (in1.k < in2.k);
    }

    if (in1.exp != in2.exp)
    {
        return in1.sign != (in1.exp < in2.exp);
    }

    if (in1.frac != in2.frac)
    {
        return in1.sign != (in1.frac < in2.frac);
    }

    return false;
}

template <int kbits, int ebits, int fbits>
bool posit_lesseqthan(const posit_unpacked<kbits, ebits, fbits> &in1, const posit_unpacked<kbits, ebits, fbits> &in2)
{
    return posit_equal(in1, in2) || posit_lessthan(in1, in2);
}

template <int kbits, int ebits, int fbits>
bool posit_morethan(const posit_unpacked<kbits, ebits, fbits> &in1, const posit_unpacked<kbits, ebits, fbits> &in2)
{
    return posit_lessthan(in2, in1);
}

template <int kbits, int ebits, int fbits>
bool posit_moreeqthan(const posit_unpacked<kbits, ebits, fbits> &in1, const posit_unpacked<kbits, ebits, fbits> &in2)
{
    return posit_equal(in1, in2) || posit_morethan(in1, in2);
}

template <int kbits, int ebits, int fbits>
posit_unpacked<kbits, ebits, fbits> posit_fabs(const posit_unpacked<kbits, ebits, fbits> &in1)
{
    posit_unpacked<kbits, ebits, fbits> result = in1;

    if (!result.is_zero && !result.is_inf)
    {
        result.sign = 0; // set sign to 0
    }

    return result;
}

template <int kbits, int ebits, int fbits>
posit_unpacked<kbits, ebits, fbits> posit_floor(const posit_unpacked<kbits, ebits, fbits> &in1)
{
    // floor to nearest
    posit_unpacked<kbits, ebits, fbits> unpacked = in1;

    ap_fixed<fbits * 2, fbits> frac = unpacked.frac;

    int exp = unpacked.getTotalExp();

    frac = frac << exp;

    if (unpacked.sign)
        frac = hls::ceil(frac);
    else
        frac = hls::floor(frac);

    frac = frac >> exp;

    unpacked.frac = frac;

    return unpacked;
}

template <int kbits, int ebits, int fbits>
posit_unpacked<kbits, ebits, fbits> posit_round(const posit_unpacked<kbits, ebits, fbits> &in1)
{
    // round to nearest
    posit_unpacked<kbits, ebits, fbits> unpacked;
    unpacked.template decode<nbits, ebits>(bits_);

    int exp = unpacked.getTotalExp();

    ap_fixed<fbits * 2, fbits> frac = unpacked.frac;
    frac = frac << exp;
    frac = hls::round(frac);
    frac = frac >> exp;

    unpacked.frac = frac;

    return unpacked;
}

template <int kbits, int ebits, int fbits>
posit_unpacked<kbits, ebits, fbits> posit_ceil(const posit_unpacked<kbits, ebits, fbits> &in1)
{
    // round to nearest
    posit_unpacked<kbits, ebits, fbits> unpacked = in1;

    int exp = unpacked.getTotalExp();

    ap_fixed<fbits * 2, fbits> frac = unpacked.frac;
    frac = frac << exp;
    frac = hls::ceil(frac);
    frac = frac >> exp;

    unpacked.frac = frac;

    return unpacked;
}

template <int nbits, int ebits>
class Posit
{
public:
    // static constexpr int max_k_size = nbits / 2;
    // static constexpr int k_bit_size = detail::clog2<nbits - 1>::value;
    static constexpr int fbits = nbits - 3 - ebits + 1;
    static constexpr int kbits = detail::clog2<nbits>::value + 1;
    // static constexpr int max_count_size = nbits / 2;

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

        bool is_zero = (c == 0);
        bool psign = c < 0;
        bool is_inf = 0;

        // mantissa is just zeros
        ap_ufixed<fbits * 2, fbits> pmantissa;
        int fexponent;

        if (c == 0)
        {
            pmantissa = 0.0;
            fexponent = 0;
        }
        else
        {
            // pmantissa[1] = 1.0;
            fexponent = 0;
            pmantissa = hls::abs(c);
            while (pmantissa >= 2.0)
            {
                pmantissa = pmantissa >> 1;
                fexponent++;
            }
        }

        posit_unpacked<kbits, ebits, fbits> unpacked;

        unpacked.setKEFromTotalExp(fexponent);

        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.is_zero = is_zero;
        unpacked.is_inf = is_inf;

        bits_ = unpacked.template encode<nbits, ebits>();
    }

    Posit(unsigned int c)
    {
        // #pragma HLS INLINE

        bool is_zero = (c == 0);
        bool psign = c < 0;
        bool is_inf = 0;

        // mantissa is just zeros
        ap_ufixed<fbits * 2, fbits> pmantissa;
        int fexponent;

        if (c == 0)
        {
            pmantissa = 0.0;
            fexponent = 0;
        }
        else
        {
            // pmantissa[1] = 1.0;
            fexponent = 0;
            pmantissa = hls::abs(c);
            while (pmantissa >= 2.0)
            {
                pmantissa = pmantissa >> 1;
                fexponent++;
            }
        }

        posit_unpacked<kbits, ebits, fbits> unpacked;

        unpacked.setKEFromTotalExp(fexponent);

        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.is_zero = is_zero;
        unpacked.is_inf = is_inf;

        bits_ = unpacked.template encode<nbits, ebits>();
    }

    Posit(float c)
    {
        // #pragma HLS INLINE

        ap_uint<32> bits = *reinterpret_cast<ap_uint<32> *>(&c);
        // unsigned int* bitsPtr = (unsigned int*)&c;
        // unsigned int bits = *bitsPtr; // Dereference to get the raw bits

        bool is_zero;
        bool is_inf;

        bool fsign;
        // ap_int<12> fexponent;
        int fexponent;
        // get mantisa from floating point
        ap_ufixed<24, 1> fmantissa;

        if (c == 0.0 || c == -0.0)
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
            fsign = bits[31];
            fexponent = bits(30, 23) - 127; // remove bias from floating point exponent
            fmantissa[23] = 1;
            fmantissa(22, 0) = bits(22, 0);
        }

        // get sign from double sign
        bool psign = fsign;

        // get mantissa from double mantissa
        ap_ufixed<fbits + 1, 2> pmantissa = fmantissa;

        posit_unpacked<kbits, ebits, fbits> unpacked;

        unpacked.setKEFromTotalExp(fexponent);

        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.is_zero = is_zero;
        unpacked.is_inf = is_inf;

        bits_ = unpacked.template encode<nbits, ebits>();
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

        if (c == 0.0 || c == -0.0)
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
        ap_ufixed<fbits + 1, 2> pmantissa = fmantissa;

        // round to nearest
        if (fmantissa[51 - (fbits - 1)] == 1)
        {
            ap_ufixed<fbits, 1> one = 0;
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

        posit_unpacked<kbits, ebits, fbits> unpacked;

        unpacked.setKEFromTotalExp(fexponent);

        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.is_zero = is_zero;
        unpacked.is_inf = is_inf;

        bits_ = unpacked.template encode<nbits, ebits>();
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

        ap_ufixed<fbits + 1, 2> pmantissa = fmantissa;

        posit_unpacked<kbits, ebits, fbits> unpacked;

        unpacked.setKEFromTotalExp(fexponent);

        unpacked.sign = psign;
        unpacked.frac = pmantissa;
        unpacked.is_zero = (c == 0.0f);
        unpacked.is_inf = (c == std::numeric_limits<float>::infinity() || c == -std::numeric_limits<float>::infinity());

        bits_ = unpacked.encode();
    }

    operator int() const
    {
        // #pragma HLS INLINE

        posit_unpacked<kbits, ebits, fbits> unpacked;
        unpacked.template decode<nbits, ebits>(bits_);

        if (unpacked.is_zero)
        {
            return 0;
        }
        else
        {
            int exp = unpacked.getTotalExp();
            int res = ap_ufixed<fbits * 2, fbits>(unpacked.frac) << exp;
            if (unpacked.sign)
            {
                res = -res;
            }
            return res;
        }
    }

    operator unsigned int() const
    {
        // #pragma HLS INLINE

        posit_unpacked<kbits, ebits, fbits> unpacked;
        unpacked.template decode<nbits, ebits>(bits_);

        if (unpacked.is_zero)
        {
            return 0;
        }
        else
        {
            int exp = unpacked.getTotalExp();
            int res = ap_fixed<fbits * 2, fbits>(unpacked.frac) << exp;
            return res;
        }
    }

    operator float() const
    {
        // #pragma HLS INLINE

        posit_unpacked<kbits, ebits, fbits> unpacked;
        unpacked.template decode<nbits, ebits>(bits_);

        ap_uint<32> bits;

        if (unpacked.is_zero)
        {
            return double(0.0);
        }

        if (unpacked.is_inf)
        {
            return std::numeric_limits<double>::infinity();
        }

        bits[31] = unpacked.sign;

        ap_uint<8> exponent;

        if (unpacked.frac[fbits - 1] == 0)
            exponent = 0;
        else
            exponent = unpacked.getTotalExp() + 127;

        bits(30, 23) = exponent;

        if (23 >= fbits - 1)
        {
            bits(22, 22 - fbits + 2) = unpacked.frac(fbits - 2, 0);
            bits(22 - fbits + 1, 0) = 0; // fill the rest with zeros
        }
        else
        {
            bits(22, 0) = unpacked.frac(fbits - 2, fbits - 2 - 22);
        }

        float fresult = *reinterpret_cast<float *>(&bits);
        return fresult;
    }

    operator double() const
    {
        // #pragma HLS INLINE

        posit_unpacked<kbits, ebits, fbits> unpacked;
        unpacked.template decode<nbits, ebits>(bits_);

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

        if (unpacked.frac[fbits - 1] == 0)
            exponent = 0;
        else
            exponent = unpacked.getTotalExp() + 1023;

        bits(62, 52) = exponent;

        if (52 >= fbits - 1)
        {
            bits(51, 51 - fbits + 2) = unpacked.frac(fbits - 2, 0);
            bits(51 - fbits + 1, 0) = 0; // fill the rest with zeros
        }
        else
        {
            bits(51, 0) = unpacked.frac(fbits - 2, fbits - 2 - 51);
        }

        double fresult = *reinterpret_cast<double *>(&bits);
        return fresult;
    }

    template <int fnbits, int fibits>
    operator ap_fixed<fnbits, fibits>() const
    {
        // #pragma HLS INLINE

        posit_unpacked<kbits, ebits, fbits> unpacked;
        unpacked.decode<nbits, ebits>(bits_);

        if (unpacked.frac == 0.0)
        {
            return 0;
        }
        else
        {
            int exp = unpacked.getTotalExp();
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
        Posit result;

        posit_unpacked<kbits, ebits, fbits> in1;
        in1.template decode<nbits, ebits>(bits_);
        if (!in1.is_zero && !in1.is_inf)
        {
            in1.sign = 0; // set sign to 0
        }

        result.bits_ = in1.template encode<nbits, ebits>();

        return result;
    }

    Posit floor() const
    {
        // floor to nearest
        posit_unpacked<kbits, ebits, fbits> unpacked;
        unpacked.template decode<nbits, ebits>(bits_);

        ap_fixed<fbits * 2, fbits> frac = unpacked.frac;

        int exp = unpacked.getTotalExp();

        frac = frac << exp;

        if (unpacked.sign)
            frac = hls::ceil(frac);
        else
            frac = hls::floor(frac);

        frac = frac >> exp;

        unpacked.frac = frac;

        Posit result;
        result.bits_ = unpacked.template encode<nbits, ebits>();

        return result;
    }

    Posit round() const
    {
        // round to nearest
        posit_unpacked<kbits, ebits, fbits> unpacked;
        unpacked.template decode<nbits, ebits>(bits_);

        int exp = unpacked.getTotalExp();

        ap_fixed<fbits * 2, fbits> frac = unpacked.frac;
        frac = frac << exp;
        frac = hls::round(frac);
        frac = frac >> exp;

        unpacked.frac = frac;

        Posit result;
        result.bits_ = unpacked.template encode<nbits, ebits>();

        return result;
    }

    Posit ceil() const
    {
        // round to nearest
        posit_unpacked<kbits, ebits, fbits> unpacked;
        unpacked.template decode<nbits, ebits>(bits_);

        int exp = unpacked.getTotalExp();

        ap_fixed<fbits * 2, fbits> frac = unpacked.frac;
        frac = frac << exp;
        frac = hls::ceil(frac);
        frac = frac >> exp;

        unpacked.frac = frac;

        Posit result;
        result.bits_ = unpacked.template encode<nbits, ebits>();

        return result;
    }

    Posit operator-() const
    {
        posit_unpacked<kbits, ebits, fbits> in1;

        in1.template decode<nbits, ebits>(bits_);

        if (!in1.is_zero)
            in1.sign = !in1.sign;

        Posit result;
        result.bits_ = in1.template encode<nbits, ebits>();

        return result;
    }

    Posit &operator+=(const Posit &rhs)
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;

        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        posit_unpacked<kbits, ebits, fbits> out = posit_adder(in1, in2);

        bits_ = out.template encode<nbits, ebits>();

        return *this;
    }

    Posit &operator-=(const Posit &rhs)
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        if (!in2.is_zero)
            in2.sign = !in2.sign;

        posit_unpacked<kbits, ebits, fbits> out = posit_adder(in1, in2);

        bits_ = out.template encode<nbits, ebits>();

        return *this;
    }

    Posit operator*=(const Posit &rhs)
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        posit_unpacked<kbits, ebits, fbits> out = posit_mult(in1, in2);

        bits_ = out.template encode<nbits, ebits>();

        return *this;
    }

    Posit operator/=(const Posit &rhs)
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        posit_unpacked<kbits, ebits, fbits> out = posit_div(in1, in2);

        bits_ = out.template encode<nbits, ebits>();

        return *this;
    }

    bool operator<(const Posit &rhs) const
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        return posit_lessthan(in1, in2);
    }

    bool operator>(const Posit &rhs) const
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        return posit_morethan(in1, in2);
    }

    bool operator<=(const Posit &rhs) const
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        return posit_lesseqthan(in1, in2);
    }

    bool operator>=(const Posit &rhs) const
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        return posit_moreeqthan(in1, in2);
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
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        posit_unpacked<kbits, ebits, fbits> out = posit_adder(in1, in2);

        Posit result;
        result.bits_ = out.template encode<nbits, ebits>();

        return result;
    }

    Posit operator-(const Posit &rhs) const
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        if (!in2.is_zero)
            in2.sign = !in2.sign;

        posit_unpacked<kbits, ebits, fbits> out = posit_adder(in1, in2);

        Posit result;
        result.bits_ = out.template encode<nbits, ebits>();

        return result;
    }

    Posit operator*(const Posit &rhs) const
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        posit_unpacked<kbits, ebits, fbits> out = posit_mult(in1, in2);

        Posit result;
        result.bits_ = out.template encode<nbits, ebits>();

        return result;
    }

    Posit operator/(const Posit &rhs) const
    {
        posit_unpacked<kbits, ebits, fbits> in1, in2;
        in1.template decode<nbits, ebits>(bits_);
        in2.template decode<nbits, ebits>(rhs.bits_);

        posit_unpacked<kbits, ebits, fbits> out = posit_div(in1, in2);

        Posit result;
        result.bits_ = out.template encode<nbits, ebits>();

        return result;
    }

private:
    ap_uint<nbits> bits_;
};

template <int nbits, int es>
Posit<nbits, es> fabs(const Posit<nbits, es> &p)
{
    posit_unpacked<Posit<nbits, es>::kbits, Posit<nbits, es>::ebits, Posit<nbits, es>::fbits> in1;
    in1.template decode<nbits, es>(p.bits_);
    in1 = posit_fabs(in1);
    Posit<nbits, es> result;
    result.bits_ = in1.template encode<nbits, es>();
    return result;
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