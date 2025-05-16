#include <iostream>
#include <stdio.h>
#include <iomanip>

#include "FloatX.h"

#define nbits 32
#define es 8

int main(void)
{
    // std::cout << std::setprecision(20) << std::fixed;

    double add_max_diff = 0.0f;
    double sub_max_diff = 0.0f;
    double mul_max_diff = 0.0f;
    double div_max_diff = 0.0f;

    double max = pow(2.0, 10);
    double step = pow(2.0, 2);

    for (double a = -max; a < max; a += step)
    {
        for (double b = -max; b < max; b += step)
        {
            double add = a + b;
            double sub = a - b;
            double mul = a * b;
            double div = a / b;

            bool failed = false;

            FloatX<nbits, es> a_mixed(a);
            double a_double = double(a_mixed);

            if (a != a_double)
            {
                std::cout << "a is not equal: " << a << " != " << a_double << std::endl;
                failed = true;
            }

            FloatX<nbits, es> b_mixed(b);
            double b_double = double(b_mixed);

            if (b != b_double)
            {
                std::cout << "b is not equal: " << b << " != " << b_double << std::endl;
                failed = true;
            }

            FloatX<nbits, es> add_mixed = a_mixed + b_mixed;
            double add_double = double(add_mixed);

            double add_diff = fabs(add - add_double);
            if (add_diff > add_max_diff)
            {
                std::cout << "diff: " << add_diff << " in: " << a << " + " << b << " = " << add << std::endl;
                std::cout << a_double << " + " << b_double << " = " << add_double << std::endl;
                failed = true;
            }

            FloatX<nbits, es> sub_mixed = a_mixed - b_mixed;
            double sub_double = double(sub_mixed);

            double mul_diff = fabs(mul - mul_double);
            if (mul_diff > mul_max_diff)
            {
                std::cout << "diff: " << mul_diff << " in: " << a << " * " << b << " = " << mul << std::endl;
                std::cout << a_double << " * " << b_double << " = " << mul_double << std::endl;
                failed = true;
            }

            FloatX<nbits, es> mul_mixed = a_mixed * b_mixed;
            double mul_double = double(mul_mixed);

            double sub_diff = fabs(sub - sub_double);
            if (sub_diff > sub_max_diff)
            {
                std::cout << "diff: " << sub_diff << " in: " << a << " - " << b << " = " << sub << std::endl;
                std::cout << a_double << " - " << b_double << " = " << sub_double << std::endl;
                failed = true;
            }

            /*
            FloatX<nbits, es> div_mixed = a_mixed / b_mixed;
            double div_double = double(div_mixed);

            double div_diff = fabs(div - div_double);
            if (div_diff > div_max_diff)
            {
                std::cout << "diff: " << div_diff << " in: " << a << " / " << b << " = " << div << std::endl;
                std::cout << a_double << " / " << b_double << " = " << div_double << std::endl;
                failed = true;
            }
            */
            if (failed)
            {
                std::cout << "failure" << std::endl;
                return 1;
            }
        }
    }

    return 0;
}
