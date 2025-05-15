#include <iostream>
#include <stdio.h>
#include <iomanip>

#include "Posit.h"

#define posit_size 16
#define exponent_size 3

int main(void)
{
    double float_sum_rmse = 0.0;
    double posit_sum_rmse = 0.0;

    for (double a = -32.0f; a < 32.0f; a += pow(2.0f, -5.0f))
    {
        for (double b = -32.0f; b < 32.0f; b += pow(2.0f, -5.0f))
        {
            float float_a = float(a);
            float float_b = float(b);
            Posit<posit_size, exponent_size> posit_a(float_a);
            Posit<posit_size, exponent_size> posit_b(float_b);

            std::cout << std::setprecision(20) << std::fixed;

            double sum = a + b;
            //double res = double(a) - double(b);
            //double mul = double(a) * double(b);
            //double div = double(a) / double(b);

            float float_sum = float_a + float_b;

            Posit<posit_size, exponent_size> posit_sum = posit_a + posit_b;
            //Posit<posit_size, exponent_size> res_mixed = a_mixed - b_mixed;
            //Posit<posit_size, exponent_size> mul_mixed = a_mixed * b_mixed;
            //Posit<posit_size, exponent_size> div_mixed = a_mixed / b_mixed;

            float_sum_rmse += std::pow(sum - double(float_sum), 2);
            posit_sum_rmse += std::pow(sum - double(posit_sum), 2);
        }
    }

    return 0;
}
