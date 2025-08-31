#include <gtest/gtest.h>
#include <cmath>
#include "hls_numerics/FloatX.h"

class FloatXTest : public ::testing::Test {
protected:
    static constexpr int nbits = 32;
    static constexpr int es = 8;
    
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FloatXTest, BasicConstruction) {
    FloatX<nbits, es> f1(3.14);
    FloatX<nbits, es> f2(2.71);
    
    // Test that construction and conversion work
    double val1 = static_cast<double>(f1);
    double val2 = static_cast<double>(f2);
    
    EXPECT_NEAR(val1, 3.14, 0.01);
    EXPECT_NEAR(val2, 2.71, 0.01);
}

TEST_F(FloatXTest, BasicArithmetic) {
    FloatX<nbits, es> a(5.0);
    FloatX<nbits, es> b(3.0);
    
    FloatX<nbits, es> sum = a + b;
    FloatX<nbits, es> diff = a - b;
    FloatX<nbits, es> prod = a * b;
    
    double sum_val = static_cast<double>(sum);
    double diff_val = static_cast<double>(diff);
    double prod_val = static_cast<double>(prod);
    
    EXPECT_NEAR(sum_val, 8.0, 0.01);
    EXPECT_NEAR(diff_val, 2.0, 0.01);
    EXPECT_NEAR(prod_val, 15.0, 0.01);
}

TEST_F(FloatXTest, ZeroAndNegative) {
    FloatX<nbits, es> zero(0.0);
    FloatX<nbits, es> negative(-5.5);
    
    double zero_val = static_cast<double>(zero);
    double neg_val = static_cast<double>(negative);
    
    EXPECT_NEAR(zero_val, 0.0, 0.001);
    EXPECT_NEAR(neg_val, -5.5, 0.01);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
