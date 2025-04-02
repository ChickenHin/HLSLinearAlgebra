#include <iostream>
#include <stdio.h>
#include <iomanip>

#include "floatX.h"

#define mantissa_size 23
#define exponent_size 8

int main(void)
{
    float sum_max_diff = 0.001f;
    float res_max_diff = 0.001f;
    float mul_max_diff = 0.001f;
    float div_max_diff = 0.001f;

    for(float a = -32.0f; a < 32.0f; a+=1.0f)//pow(2.0f, -5.0f))
    {
        for(float b = -32.0f; b < 32.0f; b+=0.01f)//pow(2.0f, -5.0f))
        {
            bool failed = false;

            floatX<mantissa_size, exponent_size> a_mixed(a);
            floatX<mantissa_size, exponent_size> b_mixed(b);

            std::cout << std::setprecision(20) << std::fixed;

            /*
            if(a != a_mixed.to_float())
            {
                std::cout << "a is not equal: " << a << " != " << a_mixed.to_float() << std::endl;
                failed = true;
            }

            if(b != b_mixed.to_float())
            {
                std::cout << "b is not equal: " << b << " != " << b_mixed.to_float() << std::endl;
                failed = true;
            }
            */

            double sum = double(a) + double(b);
            double res = double(a) - double(b);
            double mul = double(a) * double(b);
            double div = double(a) / double(b);

            floatX<mantissa_size, exponent_size> sum_mixed = a_mixed + b_mixed;
            floatX<mantissa_size, exponent_size> res_mixed = a_mixed - b_mixed;
            floatX<mantissa_size, exponent_size> mul_mixed = a_mixed * b_mixed;
            floatX<mantissa_size, exponent_size> div_mixed = a_mixed / b_mixed;

            float sum_diff = fabs(sum - float(sum_mixed));
            if(sum_diff > sum_max_diff)
            {
                std::cout << "diff: " << sum_diff << " in: " << a << " + " << b << " = " << sum << std::endl;
                std::cout << float(a_mixed) << " + " << float(b_mixed) << " = " << float(sum_mixed) << std::endl;
                failed = true;
            }

            float mul_diff = fabs(mul - float(mul_mixed));
            if(mul_diff > mul_max_diff)
            {
                std::cout << "diff: " << mul_diff << " in: " << a << " * " << b << " = " << mul << std::endl;
                std::cout << float(a_mixed) << " * " << float(b_mixed) << " = " << float(mul_mixed) << std::endl;
                failed = true;
            }
        
            float res_diff = fabs(res - float(res_mixed));
            if(res_diff > res_max_diff)
            {
                std::cout << "diff: " << res_diff << " in: " << a << " - " << b << " = " << res << std::endl;
                std::cout << float(a_mixed) << " - " << float(b_mixed) << " = " << float(res_mixed) << std::endl;
                failed = true;
            }
            
            float div_diff = fabs(div - float(div_mixed));
            if(div_diff > div_max_diff)
            {
                std::cout << "diff: " << div_diff << " in: " << a << " / " << b << " = " << div << std::endl;
                std::cout << float(a_mixed) << " / " << float(b_mixed) << " = " << float(div_mixed) << std::endl;
                failed = true;
            }

            if(failed)
            {
                std::cout << "failure" << std::endl;
                return 1;
            }
        }
    }

    return 0;
}
