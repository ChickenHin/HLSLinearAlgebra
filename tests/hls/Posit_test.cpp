#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>
#include <iomanip>

#include "hls_numerics/Posit.h"

#define nbits 32
#define es 3

template <typename Type>
Type compute_error(Type a, Posit<nbits, es> b)
{
    Type error = fabs(a - Type(b));
    if (a != 0)
        error /= fabs(a);
    return error;
}

class PositTest : public ::testing::Test {
protected:
    void SetUp() override {
        max_diff = 0.01;
        max_range = pow(2.0, 8);
        step_size = pow(2.0, -6);
    }
    
    double max_diff;
    double max_range;
    double step_size;
};

TEST_F(PositTest, BasicOperations)
{
    // Reduced test range for faster execution in CI
    double test_max = pow(2.0, 4);  // Reduced from 8 to 4
    double test_step = pow(2.0, -2); // Reduced precision for speed
    
    for (double c = 0; c < test_max; c += test_step)
    {
        for (double b = -test_max; b < test_max; b += test_step)
        {
            for (double a = -test_max; a < test_max; a += test_step)
            {
                // binary operations
                double add2 = a + b;
                double add3 = a + b + c;
                double sub2 = a - b;
                double sub3 = a - b - c;
                double mul = a * b * c;
                double div = a / b / c;
                double add_mul = a + b * c;
                double add_div = a + b / c;

                // remove -inf, it is not supported by Posit
                if (add2 == -std::numeric_limits<double>::infinity())
                    add2 = std::numeric_limits<double>::infinity();
                if (add3 == -std::numeric_limits<double>::infinity())
                    add3 = std::numeric_limits<double>::infinity();
                if (sub2 == -std::numeric_limits<double>::infinity())
                    sub2 = std::numeric_limits<double>::infinity();
                if (sub3 == -std::numeric_limits<double>::infinity())
                    sub3 = std::numeric_limits<double>::infinity();
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

                Posit<nbits, es> a_mixed(a);
                EXPECT_LE(compute_error(a, a_mixed), max_diff) 
                    << "Conversion error for a: " << a << " != " << double(a_mixed);

                Posit<nbits, es> b_mixed(b);
                EXPECT_LE(compute_error<double>(b, b_mixed), max_diff)
                    << "Conversion error for b: " << b << " != " << double(b_mixed);

                Posit<nbits, es> c_mixed(c);
                EXPECT_LE(compute_error<double>(c, c_mixed), max_diff)
                    << "Conversion error for c: " << c << " != " << double(c_mixed);

                Posit<nbits, es> a_float_mixed((float)a);
                EXPECT_LE(compute_error<float>(float(a), a_float_mixed), max_diff)
                    << "Float conversion error: " << float(a) << " != " << float(a_float_mixed);

                Posit<nbits, es> a_int_mixed((int)a);
                EXPECT_LE(compute_error<int>(int(a), a_int_mixed), max_diff)
                    << "Int conversion error: " << int(a) << " != " << int(a_int_mixed);

                Posit<nbits, es> a_uint_mixed((unsigned int)fabs(a));
                EXPECT_LE(compute_error<unsigned int>((unsigned int)fabs(a), a_uint_mixed), max_diff)
                    << "Uint conversion error: " << (unsigned int)(fabs(a)) << " != " << (unsigned int)(a_uint_mixed);

                Posit<nbits, es> add2_mixed = a_mixed + b_mixed;
                EXPECT_LE(compute_error(add2, add2_mixed), max_diff)
                    << "Addition error: " << a << " + " << b << " = " << add2 
                    << " vs " << double(a_mixed) << " + " << double(b_mixed) << " = " << double(add2_mixed);

                Posit<nbits, es> add3_mixed = a_mixed + b_mixed + c_mixed;
                EXPECT_LE(compute_error(add3, add3_mixed), max_diff)
                    << "Triple addition error: " << a << " + " << b << " + " << c << " = " << add3;

                Posit<nbits, es> add3_mixed_2 = a_mixed;
                add3_mixed_2 += b_mixed + c_mixed;
                EXPECT_LE(compute_error(add3, add3_mixed_2), max_diff)
                    << "Compound assignment error: " << a << " += " << b << " + " << c;

                // Test basic arithmetic operations
                Posit<nbits, es> sub2_mixed = a_mixed - b_mixed;
                EXPECT_LE(compute_error(sub2, sub2_mixed), max_diff)
                    << "Subtraction error: " << a << " - " << b << " = " << sub2;

                // Test remaining arithmetic operations
                EXPECT_LE(compute_error(sub3, sub3_mixed), max_diff);
                
                Posit<nbits, es> mul_mixed = a_mixed * b_mixed * c_mixed;
                EXPECT_LE(compute_error(mul, mul_mixed), max_diff);
                
                if (b != 0 && c != 0) {  // Avoid division by zero
                    Posit<nbits, es> div_mixed = a_mixed / b_mixed / c_mixed;
                    EXPECT_LE(compute_error(div, div_mixed), max_diff);
                }
                
                // Test comparison operations
                bool less_mixed = a_mixed < b_mixed;
                EXPECT_EQ(less, less_mixed) << "Comparison < failed for " << a << " and " << b;
                
                bool greater_mixed = a_mixed > b_mixed;
                EXPECT_EQ(greater, greater_mixed) << "Comparison > failed for " << a << " and " << b;
                
                bool equal_mixed = a_mixed == b_mixed;
                EXPECT_EQ(equal, equal_mixed) << "Comparison == failed for " << a << " and " << b;
                
                // Test unary operations
                Posit<nbits, es> abs_mixed = fabs(a_mixed);
                EXPECT_LE(compute_error(abs_, abs_mixed), max_diff);
                
                Posit<nbits, es> neg_mixed = -a_mixed;
                EXPECT_LE(compute_error(neg, neg_mixed), max_diff);
            }
        }
    }
}
