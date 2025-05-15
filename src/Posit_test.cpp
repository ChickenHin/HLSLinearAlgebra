#include <iostream>
#include <stdio.h>
#include <iomanip>

#include "Posit.h"

#define posit_size 16
#define exponent_size 3

int main(void)
{
    double sum_max_diff = 0.001f;
    double res_max_diff = 0.001f;
    double mul_max_diff = 0.001f;
    double div_max_diff = 0.001f;

    for (double a = -32.0f; a < 32.0f; a += pow(2.0f, -5.0f))
    {
        for (double b = -32.0f; b < 32.0f; b += pow(2.0f, -5.0f))
        {
            bool failed = false;

            Posit<posit_size, exponent_size> a_mixed(a);
            Posit<posit_size, exponent_size> b_mixed(b);

            std::cout << std::setprecision(20) << std::fixed;

            if (a != double(a_mixed))
            {
                std::cout << "a is not equal: " << a << " != " << double(a_mixed) << std::endl;
                // failed = true;
            }

            if (b != double(b_mixed))
            {
                std::cout << "b is not equal: " << b << " != " << double(b_mixed) << std::endl;
                failed = true;
            }

            double sum = a + b;
            double res = a - b;
            double mul = a * b;
            double div = a / b;

            Posit<posit_size, exponent_size> sum_mixed = a_mixed + b_mixed;
            Posit<posit_size, exponent_size> res_mixed = a_mixed - b_mixed;
            Posit<posit_size, exponent_size> mul_mixed = a_mixed * b_mixed;
            Posit<posit_size, exponent_size> div_mixed = a_mixed / b_mixed;

            double sum_diff = fabs(sum - double(sum_mixed));
            if (sum_diff > sum_max_diff)
            {
                std::cout << "diff: " << sum_diff << " in: " << a << " + " << b << " = " << sum << std::endl;
                std::cout << double(a_mixed) << " + " << double(b_mixed) << " = " << double(sum_mixed) << std::endl;
                failed = true;
            }

            double mul_diff = fabs(mul - double(mul_mixed));
            if (mul_diff > mul_max_diff)
            {
                std::cout << "diff: " << mul_diff << " in: " << a << " * " << b << " = " << mul << std::endl;
                std::cout << double(a_mixed) << " * " << double(b_mixed) << " = " << double(mul_mixed) << std::endl;
                failed = true;
            }

            double res_diff = fabs(res - double(res_mixed));
            if (res_diff > res_max_diff)
            {
                std::cout << "diff: " << res_diff << " in: " << a << " - " << b << " = " << res << std::endl;
                std::cout << double(a_mixed) << " - " << double(b_mixed) << " = " << double(res_mixed) << std::endl;
                failed = true;
            }

            double div_diff = fabs(div - double(div_mixed));
            if (div_diff > div_max_diff)
            {
                std::cout << "diff: " << div_diff << " in: " << a << " / " << b << " = " << div << std::endl;
                std::cout << double(a_mixed) << " / " << double(b_mixed) << " = " << double(div_mixed) << std::endl;
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
