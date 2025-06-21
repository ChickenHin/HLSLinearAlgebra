#include <iostream>
#include <stdio.h>
#include <iomanip>

#include "Posit.h"

#define nbits 32
#define es 3

template <typename Type>
Type compute_error(Type a, Posit<nbits, es> b)
{
    Type error = fabs(a - Type(b));
    if (a != 0)
        error /= fabs(a);
    else if (error != 0)
        error = 10000000.0f;
    return error;
}

int main(void)
{
    std::cout << std::setprecision(20) << std::fixed;

    double max_diff = 0.01;

    double max = 256.0; // pow(2.0, 8);
    double step = 0.01; // pow(2.0, -6);

    for (double c = -max; c < max; c += step)
    {
        for (double b = -max; b < max; b += step)
        {
            for (double a = -max; a < max; a += step)
            {
                // binary operations
                double add = a + b + c;
                double sub = a - b - c;
                double mul = a * b * c;
                double div = a / b / c;
                double add_mul = a + b * c;
                double add_div = a + b / c;

                // remove -inf, it is not supported by Posit
                if (add == -std::numeric_limits<double>::infinity())
                    add = std::numeric_limits<double>::infinity();
                if (sub == -std::numeric_limits<double>::infinity())
                    sub = std::numeric_limits<double>::infinity();
                if (mul == -std::numeric_limits<double>::infinity())
                    mul = std::numeric_limits<double>::infinity();
                if (div == -std::numeric_limits<double>::infinity())
                    div = std::numeric_limits<double>::infinity();

                // relational operations
                bool less = a < b;
                bool greater = a > b;
                bool less_or_equal = a <= b;
                bool greater_or_equal = a >= b;
                bool equal = a == b;
                bool not_equal = a != b;

                // unar operations
                double neg = -a;

                // other operations
                double abs_ = fabs(a);
                double round_ = round(a);
                double floor_ = floor(a);
                double ceil_ = ceil(a);

                bool failed = false;

                Posit<nbits, es> a_mixed(a);
                if (compute_error(a, a_mixed) > max_diff)
                {
                    std::cout << "a is not equal: " << a << " != " << double(a_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> b_mixed(b);
                if (compute_error<double>(b, b_mixed) > max_diff)
                {
                    std::cout << "b is not equal: " << b << " != " << double(b_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> c_mixed(c);
                if (compute_error<double>(c, c_mixed) > max_diff)
                {
                    std::cout << "c is not equal: " << b << " != " << double(c_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> a_float_mixed((float)a);
                if (compute_error<float>(float(a), a_float_mixed) > max_diff)
                {
                    std::cout << "float(a) is not equal: " << float(a) << " != " << float(a_float_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> a_int_mixed((int)a);
                if (compute_error<int>(int(a), a_int_mixed) > max_diff)
                {
                    std::cout << "int(a) is not equal: " << int(a) << " != " << int(a_int_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> a_uint_mixed((unsigned int)fabs(a));
                if (compute_error<unsigned int>((unsigned int)fabs(a), a_uint_mixed) > max_diff)
                {
                    std::cout << "(unsigned int)(a) is not equal: " << (unsigned int)(fabs(a)) << " != " << (unsigned int)(a_uint_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> add_mixed = a_mixed + b_mixed + c_mixed;
                if (compute_error(add, add_mixed) > max_diff)
                {
                    std::cout << "error in: " << a << " + " << b << " = " << add << std::endl;
                    std::cout << double(a_mixed) << " + " << double(b_mixed) << " = " << double(add_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> add_mixed_2 = a_mixed;
                add_mixed_2 += b_mixed + c_mixed;
                if (compute_error(add, add_mixed_2) > max_diff)
                {
                    std::cout << "error in: " << a << " += " << b << " = " << add << std::endl;
                    std::cout << double(a_mixed) << " += " << double(b_mixed) << " = " << double(add_mixed_2) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> sub_mixed = a_mixed - b_mixed - c_mixed;
                if (compute_error(sub, sub_mixed) > max_diff)
                {
                    std::cout << "error in: " << a << " - " << b << " = " << sub << std::endl;
                    std::cout << double(a_mixed) << " - " << double(b_mixed) << " = " << double(sub_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> sub_mixed_2 = a_mixed;
                sub_mixed_2 -= b_mixed + c_mixed;
                if (compute_error(sub, sub_mixed_2) > max_diff)
                {
                    std::cout << "error in: " << a << " -= " << b << " = " << sub << std::endl;
                    std::cout << double(a_mixed) << " -= " << double(b_mixed) << " = " << double(sub_mixed_2) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> mul_mixed = a_mixed * b_mixed * c_mixed;
                if (compute_error(mul, mul_mixed) > max_diff)
                {
                    std::cout << "error in: " << a << " * " << b << " = " << mul << std::endl;
                    std::cout << double(a_mixed) << " * " << double(b_mixed) << " = " << double(mul_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> mul_mixed_2 = a_mixed;
                mul_mixed_2 *= b_mixed * c_mixed;
                if (compute_error(mul, mul_mixed_2) > max_diff)
                {
                    std::cout << "error in: " << a << " *= " << b << " = " << mul << std::endl;
                    std::cout << double(a_mixed) << " *= " << double(b_mixed) << " = " << double(mul_mixed_2) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> div_mixed = a_mixed / b_mixed / c_mixed;
                if (compute_error(div, div_mixed) > max_diff)
                {
                    std::cout << "error in: " << a << " / " << b << " = " << div << std::endl;
                    std::cout << double(a_mixed) << " / " << double(b_mixed) << " = " << double(div_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> div_mixed_2 = a_mixed;
                div_mixed_2 /= b_mixed * c_mixed;
                if (compute_error(div, div_mixed_2) > max_diff)
                {
                    std::cout << "error in: " << a << " /= " << b << " = " << div << std::endl;
                    std::cout << double(a_mixed) << " /= " << double(b_mixed) << " = " << double(div_mixed_2) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> add_mul_mixed = a_mixed + b_mixed * c_mixed;
                if (compute_error(add_mul, add_mul_mixed) > max_diff)
                {
                    std::cout << "error in: " << a << " + " << b << " * " << c << " = " << add_mul << std::endl;
                    std::cout << double(a_mixed) << " + " << double(b_mixed) << " * " << double(c_mixed) << " = " << double(add_mul_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> add_div_mixed = a_mixed + b_mixed / c_mixed;
                if (compute_error(add_div, add_div_mixed) > max_diff)
                {
                    std::cout << "error in: " << a << " + " << b << " / " << c << " = " << add_div << std::endl;
                    std::cout << double(a_mixed) << " + " << double(b_mixed) << " / " << double(c_mixed) << " = " << double(add_div_mixed) << std::endl;
                    //failed = true;
                }

                bool less_mixed = a_mixed < b_mixed;
                if (less != less_mixed)
                {
                    std::cout << "error: in: " << a << " < " << b << " = " << less << std::endl;
                    std::cout << double(a_mixed) << " < " << double(b_mixed) << " = " << less_mixed << std::endl;
                    failed = true;
                }

                bool greater_mixed = a_mixed > b_mixed;
                if (greater != greater_mixed)
                {
                    std::cout << "error: in: " << a << " > " << b << " = " << greater << std::endl;
                    std::cout << double(a_mixed) << " > " << double(b_mixed) << " = " << greater_mixed << std::endl;
                    failed = true;
                }

                bool less_or_equal_mixed = a_mixed <= b_mixed;
                if (less_or_equal != less_or_equal_mixed)
                {
                    std::cout << "error: in: " << a << " <= " << b << " = " << less_or_equal << std::endl;
                    std::cout << double(a_mixed) << " <= " << double(b_mixed) << " = " << less_or_equal_mixed << std::endl;
                    failed = true;
                }

                bool greater_or_equal_mixed = a_mixed >= b_mixed;
                if (greater_or_equal != greater_or_equal_mixed)
                {
                    std::cout << "error: in: " << a << " >= " << b << " = " << greater_or_equal << std::endl;
                    std::cout << double(a_mixed) << " >= " << double(b_mixed) << " = " << greater_or_equal_mixed << std::endl;
                    failed = true;
                }

                bool equal_mixed = a_mixed == b_mixed;
                if (equal != equal_mixed)
                {
                    std::cout << "error: in: " << a << " == " << b << " = " << equal << std::endl;
                    std::cout << double(a_mixed) << " == " << double(b_mixed) << " = " << equal_mixed << std::endl;
                    failed = true;
                }

                bool not_equal_mixed = a_mixed != b_mixed;
                if (not_equal != not_equal_mixed)
                {
                    std::cout << "error: in: " << a << " != " << b << " = " << not_equal << std::endl;
                    std::cout << double(a_mixed) << " != " << double(b_mixed) << " = " << not_equal_mixed << std::endl;
                    failed = true;
                }

                Posit<nbits, es> abs_mixed = fabs(a_mixed);
                if (compute_error(abs_, abs_mixed) > max_diff)
                {
                    std::cout << "error: in: fabs(" << a << ") " << std::endl;
                    std::cout << abs_ << " != " << double(abs_mixed) << std::endl;
                    failed = true;
                }

                Posit<nbits, es> floor_mixed = floor(a_mixed);
                if (compute_error(floor_, floor_mixed) > max_diff)
                {
                    std::cout << "error: in: floor(" << a << ") " << std::endl;
                    std::cout << floor_ << " != " << double(floor_mixed) << std::endl;
                    // failed = true;
                }

                Posit<nbits, es> round_mixed = round(a_mixed);
                if (compute_error(round_, round_mixed) > max_diff)
                {
                    std::cout << "error: in: round(" << a << ") " << std::endl;
                    std::cout << round_ << " != " << double(round_mixed) << std::endl;
                    // failed = true;
                }

                Posit<nbits, es> ceil_mixed = ceil(a_mixed);
                if (compute_error(ceil_, ceil_mixed) > max_diff)
                {
                    std::cout << "error: in: ceil(" << a << ") " << std::endl;
                    std::cout << ceil_ << " != " << double(ceil_mixed) << std::endl;
                    // failed = true;
                }

                Posit<nbits, es> neg_mixed = -a_mixed;
                if (compute_error(neg, neg_mixed) > max_diff)
                {
                    std::cout << "error: in: -" << a << " " << std::endl;
                    std::cout << neg << " != " << double(neg_mixed) << std::endl;
                    failed = true;
                }

                if (failed)
                {
                    std::cout << "failure" << std::endl;
                    return 1;
                }
            }
        }
    }

    return 0;
}
