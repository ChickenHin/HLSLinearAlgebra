#pragma once

#include "ap_int.h"
#include <cstdint>
#include <cmath>

template <int nbits, int es>
class Posit {
public:
    ap_uint<nbits> bits;

    Posit() : bits(0) {}
    explicit Posit(uint16_t b) : bits(b) {}
    // Removed float constructor to enforce pure posit arithmetic

    bool isZero() const { return bits == 0; }
    bool isNaR() const { return bits == (ap_uint<nbits>(1) << (nbits - 1)); }

    void decode(bool& sign, int& k, int& exponent, uint64_t& fraction, int& frac_len) const {
        if (isZero() || isNaR()) {
            sign = false; k = 0; exponent = 0; fraction = 0; frac_len = 0;
            return;
        }

        ap_uint<nbits> ui = bits;
        sign = ui[nbits - 1];
        if (sign) ui = (~ui + 1);

        int reg_len = 1;
        bool reg_bit = ui[nbits - 2];
        while (reg_len < nbits - 1 && ui[nbits - 2 - reg_len] == reg_bit) {
            reg_len++;
        }
        k = reg_bit ? (reg_len - 1) : (-reg_len);

        int exp_start = nbits - 2 - reg_len;
        exponent = 0;
        for (int i = 0; i < es; ++i) {
            int bit_pos = exp_start - i;
            if (bit_pos >= 0)
                exponent |= (ui[bit_pos] << (es - 1 - i));
        }

        int frac_start = exp_start - es;
        frac_len = frac_start + 1;
        fraction = 0;
        for (int i = 0; i < frac_len; ++i) {
            int bit_pos = frac_start - i;
            if (bit_pos >= 0)
                fraction |= (uint64_t(ui[bit_pos]) << (frac_len - 1 - i));
        }
    }

    static Posit encode(bool sign, int k, int exponent, uint64_t fraction, int frac_len) {
        if (k > nbits - 2) {
            ap_uint<nbits> maxpos = (ap_uint<nbits>(1) << (nbits - 1)) - 1;

            ap_int<nbits + 2> sval;
            if (sign) {
                sval = -ap_int<nbits + 2>(maxpos);
            } else {
                sval = ap_int<nbits + 2>(maxpos);
            }

            ap_uint<nbits> val = sval.range(nbits - 1, 0);
            return Posit(uint16_t(val));
        }
        if (k < -(nbits - 2)) {
            return Posit(uint16_t(0));
        }

        ap_uint<nbits> result = 0;
        int reg_len = std::abs(k) + 1;
        if (reg_len > nbits - 1) reg_len = nbits - 1;

        if (k >= 0) {
            for (int i = 0; i < reg_len; ++i)
                result.set(nbits - 2 - i, 1);
            if (nbits - 2 - reg_len >= 0)
                result.set(nbits - 2 - reg_len, 0);
        } else {
            for (int i = 0; i < reg_len; ++i)
                result.set(nbits - 2 - i, 0);
            if (nbits - 2 - reg_len >= 0)
                result.set(nbits - 2 - reg_len, 1);
        }

        int exp_pos = nbits - 2 - reg_len - 1;
        for (int i = 0; i < es; ++i) {
            int bit_pos = exp_pos - i;
            if (bit_pos >= 0) {
                bool bit = (exponent >> (es - 1 - i)) & 1;
                result.set(bit_pos, bit);
            }
        }

        int frac_pos = exp_pos - es;
        for (int i = 0; i < frac_len; ++i) {
            int bit_pos = frac_pos - i;
            if (bit_pos >= 0) {
                bool bit = (fraction >> (frac_len - 1 - i)) & 1;
                result.set(bit_pos, bit);
            }
        }

        if (sign) {
            result = (~result + 1);
        }

        return Posit(uint16_t(result));
    }

    static uint64_t round_fraction(uint64_t value, int shift, int frac_bits) {
        if (shift <= 0) return value << (-shift);
        uint64_t mask = (1ull << shift) - 1;
        uint64_t halfway = 1ull << (shift - 1);
        uint64_t lsb = (value >> shift) & 1;
        bool round = ((value & mask) > halfway) ||
                     ((value & mask) == halfway && lsb);
        return (value >> shift) + round;
    }

    Posit operator+(const Posit& rhs) const {
        if (isNaR() || rhs.isNaR()) return Posit(uint16_t(1 << (nbits - 1)));
        if (isZero()) return rhs;
        if (rhs.isZero()) return *this;

        bool signA, signB;
        int kA, kB, expA, expB;
        uint64_t fracA, fracB;
        int fracLenA, fracLenB;
        decode(signA, kA, expA, fracA, fracLenA);
        rhs.decode(signB, kB, expB, fracB, fracLenB);

        int scaleA = kA * (1 << es) + expA;
        int scaleB = kB * (1 << es) + expB;

        uint64_t mantA = (uint64_t(1) << fracLenA) | fracA;
        uint64_t mantB = (uint64_t(1) << fracLenB) | fracB;

        int scaleDiff = scaleA - scaleB;
        if (scaleDiff > 0) {
            if (scaleDiff > 64) mantB = 0;
            else mantB = mantB >> scaleDiff;
            scaleB = scaleA;
        } else if (scaleDiff < 0) {
            int shift = -scaleDiff;
            if (shift > 64) mantA = 0;
            else mantA = mantA >> shift;
            scaleA = scaleB;
        }

        uint64_t resMant;
        bool resSign;
        if (signA == signB) {
            resMant = mantA + mantB;
            resSign = signA;
        } else {
            if (mantA >= mantB) {
                resMant = mantA - mantB;
                resSign = signA;
            } else {
                resMant = mantB - mantA;
                resSign = signB;
            }
            if (resMant == 0) return Posit(0);
        }

        int resFracLen = std::max(fracLenA, fracLenB) + 1;
        int leading_zeros = 0;
        while (((resMant & (uint64_t(1) << (resFracLen))) == 0) && resFracLen > 0) {
            resMant <<= 1;
            leading_zeros++;
            resFracLen--;
        }

        int resScale = scaleA - leading_zeros;

        int max_exponent = (1 << es) - 1;
        int res_k = resScale >> es;
        int res_exp = resScale & max_exponent;

        int frac_bits = nbits - 1 - (std::abs(res_k) + 1) - es;
        if (frac_bits < 0) frac_bits = 0;

        int shift = resFracLen - frac_bits;
        uint64_t rounded_frac = round_fraction(resMant, shift, frac_bits);

        if (rounded_frac >= (uint64_t(1) << frac_bits)) {
            rounded_frac >>= 1;
            res_exp++;
            if (res_exp > max_exponent) {
                res_exp = 0;
                res_k++;
            }
        }

        return encode(resSign, res_k, res_exp, rounded_frac, frac_bits);
    }

    Posit operator-(const Posit& rhs) const {
        Posit neg_rhs = rhs;
        neg_rhs.bits = (~rhs.bits + 1);
        return *this + neg_rhs;
    }

    Posit operator*(const Posit& rhs) const {
        if (isNaR() || rhs.isNaR()) return Posit(uint16_t(1 << (nbits - 1)));
        if (isZero() || rhs.isZero()) return Posit(uint16_t(0));

        bool signA, signB;
        int kA, kB, expA, expB;
        uint64_t fracA, fracB;
        int fracLenA, fracLenB;
        decode(signA, kA, expA, fracA, fracLenA);
        rhs.decode(signB, kB, expB, fracB, fracLenB);

        bool res_sign = signA ^ signB;
        int res_k = kA + kB;
        int res_exp = expA + expB;

        uint64_t mantA = (uint64_t(1) << fracLenA) | fracA;
        uint64_t mantB = (uint64_t(1) << fracLenB) | fracB;
        uint64_t product = mantA * mantB;
        int product_len = fracLenA + fracLenB + 1;

        if (product & (uint64_t(1) << product_len)) {
            product >>= 1;
            res_exp++;
        }

        int max_exponent = (1 << es) - 1;
        if (res_exp > max_exponent) {
            res_exp -= (max_exponent + 1);
            res_k++;
        }

        int frac_bits = nbits - 1 - (std::abs(res_k) + 1) - es;
        if (frac_bits < 0) frac_bits = 0;
        int shift = product_len - frac_bits;
        uint64_t res_frac = round_fraction(product, shift, frac_bits);

        if (res_frac >= (uint64_t(1) << frac_bits)) {
            res_frac >>= 1;
            res_exp++;
            if (res_exp > max_exponent) {
                res_exp = 0;
                res_k++;
            }
        }

        return encode(res_sign, res_k, res_exp, res_frac, frac_bits);
    }

    Posit operator/(const Posit& rhs) const {
        if (rhs.isZero() || isNaR() || rhs.isNaR()) return Posit(uint16_t(1 << (nbits - 1)));
        if (isZero()) return Posit(0);

        bool signA, signB;
        int kA, kB, expA, expB;
        uint64_t fracA, fracB;
        int fracLenA, fracLenB;
        decode(signA, kA, expA, fracA, fracLenA);
        rhs.decode(signB, kB, expB, fracB, fracLenB);

        bool res_sign = signA ^ signB;
        int res_k = kA - kB;
        int res_exp = expA - expB;

        uint64_t mantA = (uint64_t(1) << fracLenA) | fracA;
        uint64_t mantB = (uint64_t(1) << fracLenB) | fracB;

        int shift = 63 - fracLenA;
        uint64_t dividend = mantA << shift;
        uint64_t quotient = dividend / mantB;
        int quotient_len = 64;

        while (((quotient & (uint64_t(1) << (quotient_len - 1))) == 0) && quotient_len > 0) {
            quotient <<= 1;
            quotient_len--;
        }

        int max_exponent = (1 << es) - 1;
        if (res_exp < 0) {
            while (res_exp < 0 && res_k > -(nbits - 2)) {
                res_exp += max_exponent + 1;
                res_k--;
            }
            if (res_exp < 0) {
                return Posit(0);
            }
        }

        int frac_bits = nbits - 1 - (std::abs(res_k) + 1) - es;
        if (frac_bits < 0) frac_bits = 0;

        int round_shift = quotient_len - frac_bits;
        uint64_t res_frac = round_fraction(quotient, round_shift, frac_bits);

        if (res_frac >= (uint64_t(1) << frac_bits)) {
            res_frac >>= 1;
            res_exp++;
            if (res_exp > max_exponent) {
                res_exp = 0;
                res_k++;
            }
        }

        return encode(res_sign, res_k, res_exp, res_frac, frac_bits);
    }

    // For debugging only: convert posit to float (not used internally)
    float to_float() const {
        if (isZero()) return 0.0f;
        if (isNaR()) return std::numeric_limits<float>::quiet_NaN();

        bool sign;
        int k, exponent;
        uint64_t frac;
        int frac_len;
        decode(sign, k, exponent, frac, frac_len);

        int scale = k * (1 << es) + exponent;
        double value = 1.0;
        for (int i = 0; i < frac_len; ++i) {
            if ((frac >> (frac_len - 1 - i)) & 1) {
                value += std::ldexp(1.0, -(i + 1));
            }
        }
        value = std::ldexp(value, scale);
        return sign ? float(-value) : float(value);
    }
};
