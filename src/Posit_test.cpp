#include <iostream>
#include <stdio.h>
#include <iomanip>

#include "Posit.h"

#define nbits 32
#define es 3

int main(void)
{
    std::cout << std::setprecision(20) << std::fixed;

    double add_max_diff = 0.0f;
    double sub_max_diff = 0.0f;
    double mul_max_diff = 0.0f;
    double div_max_diff = 0.1f;

    double max = pow(2.0, 6);
    double step = pow(2.0, -2);

    for (double a = -max; a < max; a += step)
    {
        for (double b = -max; b < max; b += step)
        {
            // binary operations
            double add = a + b;
            double sub = a - b;
            double mul = a * b;
            double div = a / b;

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
            double neg = -b;

            // other operations
            double abs_ = fabs(b);
            double round_ = round(b);
            double floor_ = floor(b);
            double ceil_ = ceil(b);

            bool failed = false;

            Posit<nbits, es> a_mixed(a);
            double a_double = double(a_mixed);

            if (a != a_double)
            {
                std::cout << "a is not equal: " << a << " != " << a_double << std::endl;
                // failed = true;
            }

            Posit<nbits, es> b_mixed(b);
            Posit<nbits, es> b_flaot_mixed((float)b);
            Posit<nbits, es> b_int_mixed((int)b);
            Posit<nbits, es> b_uint_mixed((unsigned int)fabs(b));

            double b_double = double(b_mixed);
            float b_float = float(b_flaot_mixed);
            int b_int = int(b_int_mixed);
            unsigned int b_uint = (unsigned int)(b_uint_mixed);

            if (b != b_double)
            {
                std::cout << "b is not equal: " << b << " != " << b_double << std::endl;
                failed = true;
            }

            if (float(b) != b_float)
            {
                std::cout << "float(b) is not equal: " << float(b) << " != " << b_float << std::endl;
                failed = true;
            }

            if (int(b) != b_int)
            {
                std::cout << "int(b) is not equal: " << int(b) << " != " << b_int << std::endl;
                failed = true;
            }

            if ((unsigned int)(fabs(b)) != b_uint)
            {
                std::cout << "(unsigned int)(b) is not equal: " << (unsigned int)(fabs(b)) << " != " << b_uint << std::endl;
                failed = true;
            }

            Posit<nbits, es> add_mixed = a_mixed + b_mixed;
            double add_double = double(add_mixed);

            double add_diff = fabs(add - add_double);
            if (add_diff > add_max_diff)
            {
                std::cout << "diff: " << add_diff << " in: " << a << " + " << b << " = " << add << std::endl;
                std::cout << a_double << " + " << b_double << " = " << add_double << std::endl;
                failed = true;
            }

            Posit<nbits, es> add_mixed_2 = a_mixed;
            add_mixed_2 += b_mixed;
            double add_double_2 = double(add_mixed_2);

            double add_diff_2 = fabs(add - add_double_2);
            if (add_diff_2 > add_max_diff)
            {
                std::cout << "diff: " << add_diff_2 << " in: " << a << " += " << b << " = " << add << std::endl;
                std::cout << a_double << " += " << b_double << " = " << add_double_2 << std::endl;
                failed = true;
            }

            Posit<nbits, es> sub_mixed = a_mixed - b_mixed;
            double sub_double = double(sub_mixed);

            double sub_diff = fabs(sub - sub_double);
            if (sub_diff > sub_max_diff)
            {
                std::cout << "diff: " << sub_diff << " in: " << a << " - " << b << " = " << sub << std::endl;
                std::cout << a_double << " - " << b_double << " = " << sub_double << std::endl;
                failed = true;
            }

            Posit<nbits, es> sub_mixed_2 = a_mixed - b_mixed;
            double sub_double_2 = double(sub_mixed_2);

            double sub_diff_2 = fabs(sub - sub_double_2);
            if (sub_diff_2 > sub_max_diff)
            {
                std::cout << "diff: " << sub_diff_2 << " in: " << a << " -= " << b << " = " << sub << std::endl;
                std::cout << a_double << " -= " << b_double << " = " << sub_double_2 << std::endl;
                failed = true;
            }

            Posit<nbits, es> mul_mixed = a_mixed * b_mixed;
            double mul_double = double(mul_mixed);

            double mul_diff = fabs(mul - mul_double);
            if (mul_diff > mul_max_diff)
            {
                std::cout << "diff: " << mul_diff << " in: " << a << " * " << b << " = " << mul << std::endl;
                std::cout << a_double << " * " << b_double << " = " << mul_double << std::endl;
                failed = true;
            }

            Posit<nbits, es> mul_mixed_2 = a_mixed;
            mul_mixed_2 *= b_mixed;
            double mul_double_2 = double(mul_mixed_2);

            double mul_diff_2 = fabs(mul - mul_double_2);
            if (mul_diff_2 > mul_max_diff)
            {
                std::cout << "diff: " << mul_diff_2 << " in: " << a << " *= " << b << " = " << mul << std::endl;
                std::cout << a_double << " *= " << b_double << " = " << mul_double_2 << std::endl;
                failed = true;
            }

            Posit<nbits, es> div_mixed = a_mixed / b_mixed;
            double div_double = double(div_mixed);

            double div_diff = fabs(div - div_double);
            if (div_diff > div_max_diff)
            {
                std::cout << "diff: " << div_diff << " in: " << a << " / " << b << " = " << div << std::endl;
                std::cout << a_double << " / " << b_double << " = " << div_double << std::endl;
                failed = true;
            }

            bool less_mixed = a_mixed < b_mixed;
            if (less != less_mixed)
            {
                std::cout << "error: in: " << a << " < " << b << " = " << less << std::endl;
                std::cout << a_double << " < " << b_double << " = " << less_mixed << std::endl;
                failed = true;
            }

            bool greater_mixed = a_mixed > b_mixed;
            if (greater != greater_mixed)
            {
                std::cout << "error: in: " << a << " > " << b << " = " << greater << std::endl;
                std::cout << a_double << " > " << b_double << " = " << greater_mixed << std::endl;
                failed = true;
            }

            bool less_or_equal_mixed = a_mixed <= b_mixed;
            if (less_or_equal != less_or_equal_mixed)
            {
                std::cout << "error: in: " << a << " <= " << b << " = " << less_or_equal << std::endl;
                std::cout << a_double << " <= " << b_double << " = " << less_or_equal_mixed << std::endl;
                failed = true;
            }

            bool greater_or_equal_mixed = a_mixed >= b_mixed;
            if (greater_or_equal != greater_or_equal_mixed)
            {
                std::cout << "error: in: " << a << " >= " << b << " = " << greater_or_equal << std::endl;
                std::cout << a_double << " >= " << b_double << " = " << greater_or_equal_mixed << std::endl;
                failed = true;
            }

            bool equal_mixed = a_mixed == b_mixed;
            if (equal != equal_mixed)
            {
                std::cout << "error: in: " << a << " == " << b << " = " << equal << std::endl;
                std::cout << a_double << " == " << b_double << " = " << equal_mixed << std::endl;
                failed = true;
            }

            bool not_equal_mixed = a_mixed != b_mixed;
            if (not_equal != not_equal_mixed)
            {
                std::cout << "error: in: " << a << " != " << b << " = " << not_equal << std::endl;
                std::cout << a_double << " != " << b_double << " = " << not_equal_mixed << std::endl;
                failed = true;
            }

            Posit<nbits, es> abs_mixed = fabs(b_mixed);
            double abs_double = double(abs_mixed);
            if (abs_ != abs_double)
            {
                std::cout << "error: in: fabs(" << b << ") " << std::endl;
                std::cout << abs_ << " != " << abs_double << std::endl;
                failed = true;
            }

            Posit<nbits, es> floor_mixed = floor(b_mixed);
            double floor_double = double(floor_mixed);
            if (floor_ != floor_double)
            {
                std::cout << "error: in: floor(" << b << ") " << std::endl;
                std::cout << floor_ << " != " << floor_double << std::endl;
                failed = true;
            }

            Posit<nbits, es> round_mixed = round(b_mixed);
            double round_double = double(round_mixed);
            if (round_ != round_double)
            {
                std::cout << "error: in: round(" << b << ") " << std::endl;
                std::cout << round_ << " != " << round_double << std::endl;
                failed = true;
            }

            Posit<nbits, es> ceil_mixed = ceil(b_mixed);
            double ceil_double = double(ceil_mixed);
            if (ceil_ != ceil_double)
            {
                std::cout << "error: in: ceil(" << b << ") " << std::endl;
                std::cout << ceil_ << " != " << ceil_double << std::endl;
                //failed = true;
            }

            Posit<nbits, es> neg_mixed = -b_mixed;
            double neg_double = double(neg_mixed);
            if (neg != neg_double)
            {
                std::cout << "error: in: -" << b << " " << std::endl;
                std::cout << neg << " != " << neg_double << std::endl;
                failed = true;
            }

            if (failed)
            {
                std::cout << "failure" << std::endl;
                return 1;
            }
        }
    }

    return 0;
}
