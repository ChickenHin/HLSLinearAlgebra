#include <gtest/gtest.h>
#include <cmath>

// Basic CPU test to verify GoogleTest integration works
// This bypasses all HLS dependencies to establish a working baseline

class BasicCPUTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(BasicCPUTest, BasicArithmetic) {
    double a = 5.0;
    double b = 3.0;
    
    EXPECT_NEAR(a + b, 8.0, 0.001);
    EXPECT_NEAR(a - b, 2.0, 0.001);
    EXPECT_NEAR(a * b, 15.0, 0.001);
    EXPECT_NEAR(a / b, 5.0/3.0, 0.001);
}

TEST_F(BasicCPUTest, MathFunctions) {
    EXPECT_NEAR(std::sqrt(16.0), 4.0, 0.001);
    EXPECT_NEAR(std::sin(0.0), 0.0, 0.001);
    EXPECT_NEAR(std::cos(0.0), 1.0, 0.001);
    EXPECT_NEAR(std::pow(2.0, 3.0), 8.0, 0.001);
}

TEST_F(BasicCPUTest, IntegerOperations) {
    int x = 42;
    int y = 7;
    
    EXPECT_EQ(x + y, 49);
    EXPECT_EQ(x - y, 35);
    EXPECT_EQ(x * y, 294);
    EXPECT_EQ(x / y, 6);
    EXPECT_EQ(x % y, 0);
}

TEST_F(BasicCPUTest, BitwiseOperations) {
    uint64_t a = 0xFF00;
    uint64_t b = 0x0F0F;
    
    EXPECT_EQ(a & b, 0x0F00);
    EXPECT_EQ(a | b, 0xFF0F);
    EXPECT_EQ(a ^ b, 0xF00F);
    EXPECT_EQ(a >> 4, 0x0FF0);
    EXPECT_EQ(b << 4, 0xF0F0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
