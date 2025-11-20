#include <gtest/gtest.h>
#include <cmath>
#include "linalg/linalg.h"

class LinalgTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(LinalgTest, MatrixBasicOperations) {
    // Test basic matrix operations with double type
    linalg::Mat<double, 3, 3> mat1;
    linalg::Mat<double, 3, 3> mat2;
    
    // Test Zero constructor
    auto zero_mat = linalg::Mat<double, 3, 3>::Zero();
    
    // Verify zero matrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_NEAR(zero_mat(i, j), 0.0, 1e-10);
        }
    }
}

TEST_F(LinalgTest, MatrixIdentity) {
    // Test Identity matrix creation
    auto identity_mat = linalg::Mat<double, 3, 3>::Identity();
    
    // Verify identity matrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j) {
                EXPECT_NEAR(identity_mat(i, j), 1.0, 1e-10);
            } else {
                EXPECT_NEAR(identity_mat(i, j), 0.0, 1e-10);
            }
        }
    }
}

TEST_F(LinalgTest, MatrixArithmetic) {
    // Test matrix arithmetic operations
    linalg::Mat<double, 2, 2> mat1;
    linalg::Mat<double, 2, 2> mat2;
    
    // Initialize matrices
    mat1(0, 0) = 1.0; mat1(0, 1) = 2.0;
    mat1(1, 0) = 3.0; mat1(1, 1) = 4.0;
    
    mat2(0, 0) = 5.0; mat2(0, 1) = 6.0;
    mat2(1, 0) = 7.0; mat2(1, 1) = 8.0;
    
    // Test addition
    auto sum = mat1 + mat2;
    EXPECT_NEAR(sum(0, 0), 6.0, 1e-10);
    EXPECT_NEAR(sum(0, 1), 8.0, 1e-10);
    EXPECT_NEAR(sum(1, 0), 10.0, 1e-10);
    EXPECT_NEAR(sum(1, 1), 12.0, 1e-10);
    
    // Test subtraction
    auto diff = mat2 - mat1;
    EXPECT_NEAR(diff(0, 0), 4.0, 1e-10);
    EXPECT_NEAR(diff(0, 1), 4.0, 1e-10);
    EXPECT_NEAR(diff(1, 0), 4.0, 1e-10);
    EXPECT_NEAR(diff(1, 1), 4.0, 1e-10);
}

TEST_F(LinalgTest, VectorOperations) {
    // Test vector operations (using 1D matrices)
    linalg::Mat<double, 4, 1> vec1;
    linalg::Mat<double, 4, 1> vec2;
    
    // Initialize vectors using single-index operator for vectors
    vec1(0, 0) = 1.0;
    vec1(1, 0) = 2.0;
    vec1(2, 0) = 3.0;
    vec1(3, 0) = 4.0;
    
    vec2(0, 0) = 4.0;
    vec2(1, 0) = 5.0;
    vec2(2, 0) = 6.0;
    
    // Test vector addition
    auto sum = vec1 + vec2;
    EXPECT_NEAR(sum(0, 0), 5.0, 1e-10);
    EXPECT_NEAR(sum(1, 0), 7.0, 1e-10);
    EXPECT_NEAR(sum(2, 0), 9.0, 1e-10);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
