#include <iostream>
#include <stdio.h>
#include "hls_numerics/FloatX.h"
#include "hls_numerics/Posit.h"
#include "linalg/linalg.h"

#define float_bits 32
#define float_es 8
#define posit_bits 32
#define posit_es 3

int main(void)
{
    // std::cout << std::setprecision(20) << std::fixed;

    double float_rep_rmse = 0.0;
    double half_rep_rmse = 0.0;
    double floatx_rep_rmse = 0.0;
    double posit_rep_rmse = 0.0;

    double float_sum_rmse = 0.0;
    double half_sum_rmse = 0.0;
    double floatx_sum_rmse = 0.0;
    double posit_sum_rmse = 0.0;

    double float_sub_rmse = 0.0;
    double half_sub_rmse = 0.0;
    double floatx_sub_rmse = 0.0;
    double posit_sub_rmse = 0.0;

    double float_mul_rmse = 0.0;
    double half_mul_rmse = 0.0;
    double floatx_mul_rmse = 0.0;
    double posit_mul_rmse = 0.0;

    double float_div_rmse = 0.0;
    double half_div_rmse = 0.0;
    double floatx_div_rmse = 0.0;
    double posit_div_rmse = 0.0;

    double count = 0;

    double max = pow(2.0, pow(2.0, 3));//pow(2.0, 10);
    double step = max/100.0;//pow(2.0, 1);

    for (double a = -max; a < max; a += step)
    {
        // std::cout << "progress: " << 100.0*std::fabs(a + max)/(2.0*max) << "\t\r" << std::flush;
        std::cout << "progress: " << 100.0 * std::fabs(a + max) / (2.0 * max) << std::endl;
        for (double b = -max; b < max; b += step)
        {
            double sum = a + b;
            double sub = a - b;
            double mul = a * b;
            double div = a / b;

            float float_a = float(a);
            double float_a_d = double(float_a);

            float float_b = float(b);
            double float_b_d = double(float_b);

            half half_a = half(a);
            double half_a_d = double(half_a);

            half half_b = half(b);
            double half_b_d = double(half_b);

            FloatX<float_bits, float_es> floatx_a(a);
            double floatx_a_d = double(floatx_a);

            FloatX<float_bits, float_es> floatx_b(b);
            double floatx_b_d = double(floatx_b);

            Posit<posit_bits, posit_es> posit_a(a);
            double posit_a_d = double(posit_a);

            Posit<posit_bits, posit_es> posit_b(b);
            double posit_b_d = double(posit_b);

            float float_sum = float_a + float_b;
            double float_sum_d = double(float_sum);

            float float_sub = float_a - float_b;
            double float_sub_d = double(float_sub);

            float float_mul = float_a * float_b;
            double float_mul_d = double(float_mul);

            // float float_div = float_a / float_b;
            // double float_div_d = double(float_div);

            half half_sum = half_a + half_b;
            double half_sum_d = double(half_sum);

            half half_sub = half_a - half_b;
            double half_sub_d = double(half_sub);

            half half_mul = half_a * half_b;
            double half_mul_d = double(half_mul);

            // half half_div = half_a / half_b;
            // double half_div_d = double(half_div);

            FloatX<float_bits, float_es> floatx_sum = floatx_a + floatx_b;
            double floatx_sum_d = double(floatx_sum);

            FloatX<float_bits, float_es> floatx_sub = floatx_a - floatx_b;
            double floatx_sub_d = double(floatx_sub);

            FloatX<float_bits, float_es> floatx_mul = floatx_a * floatx_b;
            double floatx_mul_d = double(floatx_mul);

            // FloatX<float_bits, float_es> floatx_div = floatx_a / floatx_b;
            // double floatx_div_d = double(floatx_div);

            Posit<posit_bits, posit_es> posit_sum = posit_a + posit_b;
            double posit_sum_d = double(posit_sum);

            Posit<posit_bits, posit_es> posit_sub = posit_a - posit_b;
            double posit_sub_d = double(posit_sub);

            Posit<posit_bits, posit_es> posit_mul = posit_a * posit_b;
            double posit_mul_d = double(posit_mul);

            // Posit<posit_size, exponent_size> posit_div = posit_a / posit_b;
            // double posit_div_d = double(posit_div);

            float_rep_rmse += std::pow(a - float_a_d, 2) + std::pow(b - float_b_d, 2);
            half_rep_rmse += std::pow(a - half_a_d, 2) + std::pow(b - half_b_d, 2);
            floatx_rep_rmse += std::pow(a - floatx_a_d, 2) + std::pow(b - floatx_b_d, 2);
            posit_rep_rmse += std::pow(a - posit_a_d, 2) + std::pow(b - posit_b_d, 2);

            float_sum_rmse += std::pow(sum - float_sum_d, 2);
            half_sum_rmse += std::pow(sum - half_sum_d, 2);
            floatx_sum_rmse += std::pow(sum - floatx_sum_d, 2);
            posit_sum_rmse += std::pow(sum - posit_sum_d, 2);

            float_sub_rmse += std::pow(sub - float_sub_d, 2);
            half_sub_rmse += std::pow(sub - half_sub_d, 2);
            floatx_sub_rmse += std::pow(sub - floatx_sub_d, 2);
            posit_sub_rmse += std::pow(sub - posit_sub_d, 2);

            float_mul_rmse += std::pow(mul - float_mul_d, 2);
            half_mul_rmse += std::pow(mul - half_mul_d, 2);
            floatx_mul_rmse += std::pow(mul - floatx_mul_d, 2);
            posit_mul_rmse += std::pow(mul - posit_mul_d, 2);

            // float_div_rmse += std::pow(div - double(float_div), 2);
            // floatx_div_rmse += std::pow(div - double(floatx_div), 2);
            // posit_div_rmse += std::pow(div - double(posit_div), 2);

            count++;
        }
    }

    float_rep_rmse = std::sqrt(float_rep_rmse / (2 * count));
    half_rep_rmse = std::sqrt(half_rep_rmse / (2 * count));
    floatx_rep_rmse = std::sqrt(floatx_rep_rmse / (2 * count));
    posit_rep_rmse = std::sqrt(posit_rep_rmse / (2 * count));

    float_sum_rmse = std::sqrt(float_sum_rmse / count);
    half_sum_rmse = std::sqrt(half_sum_rmse / count);
    floatx_sum_rmse = std::sqrt(floatx_sum_rmse / count);
    posit_sum_rmse = std::sqrt(posit_sum_rmse / count);

    float_sub_rmse = std::sqrt(float_sub_rmse / count);
    half_sub_rmse = std::sqrt(half_sub_rmse / count);
    floatx_sub_rmse = std::sqrt(floatx_sub_rmse / count);
    posit_sub_rmse = std::sqrt(posit_sub_rmse / count);

    float_mul_rmse = std::sqrt(float_mul_rmse / count);
    half_mul_rmse = std::sqrt(half_mul_rmse / count);
    floatx_mul_rmse = std::sqrt(floatx_mul_rmse / count);
    posit_mul_rmse = std::sqrt(posit_mul_rmse / count);

    float_div_rmse = std::sqrt(float_div_rmse / count);
    half_div_rmse = std::sqrt(half_div_rmse / count);
    floatx_div_rmse = std::sqrt(floatx_div_rmse / count);
    posit_div_rmse = std::sqrt(posit_div_rmse / count);

    std::cout << "float rep rmse: " << float_rep_rmse << std::endl;
    std::cout << "half rep rmse: " << half_rep_rmse << std::endl;
    std::cout << "floatx rep rmse: " << floatx_rep_rmse << std::endl;
    std::cout << "posit rep rmse: " << posit_rep_rmse << std::endl;

    std::cout << "float sum rmse: " << float_sum_rmse << std::endl;
    std::cout << "half sum rmse: " << half_sum_rmse << std::endl;
    std::cout << "floatx sum rmse: " << floatx_sum_rmse << std::endl;
    std::cout << "posit sum rmse: " << posit_sum_rmse << std::endl;

    std::cout << "float sub rmse: " << float_sub_rmse << std::endl;
    std::cout << "half sub rmse: " << half_sub_rmse << std::endl;
    std::cout << "floatx sub rmse: " << floatx_sub_rmse << std::endl;
    std::cout << "posit sub rmse: " << posit_sub_rmse << std::endl;

    std::cout << "float mul rmse: " << float_mul_rmse << std::endl;
    std::cout << "half mul rmse: " << half_mul_rmse << std::endl;
    std::cout << "floatx mul rmse: " << floatx_mul_rmse << std::endl;
    std::cout << "posit mul rmse: " << posit_mul_rmse << std::endl;

    std::cout << "float div rmse: " << float_div_rmse << std::endl;
    std::cout << "half div rmse: " << half_div_rmse << std::endl;
    std::cout << "floatx div rmse: " << floatx_div_rmse << std::endl;
    std::cout << "posit div rmse: " << posit_div_rmse << std::endl;

    std::cout << "end " << std::endl;

    return 0;
}
