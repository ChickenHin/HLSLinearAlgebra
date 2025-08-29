#include <gtest/gtest.h>
#include <cmath>
#include "linalg/linalg.h"

class NumericalPrecisionTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    const double TOLERANCE = 1e-10;
    const double LOOSE_TOLERANCE = 1e-6;
};

// ============================================================================
// Edge Cases and Numerical Precision Tests
// ============================================================================

TEST_F(NumericalPrecisionTest, EdgeCases) {
    // Very small numbers
    linalg::Vec3<double> tiny(1e-15, 1e-15, 1e-15);
    double norm = tiny.norm();
    EXPECT_GT(norm, 0.0);
    EXPECT_LT(norm, 1e-14);
    
    // Very large numbers
    linalg::Vec3<double> large(1e10, 1e10, 1e10);
    double large_norm = large.norm();
    EXPECT_NEAR(large_norm, std::sqrt(3.0) * 1e10, large_norm * 1e-10);
    
    // Mixed scales
    linalg::Vec3<double> mixed(1e-10, 1.0, 1e10);
    double mixed_norm = mixed.norm();
    EXPECT_GT(mixed_norm, 1e10 - 1.0);
    
    // Zero matrix operations
    auto zero_mat = linalg::Mat3<double>::Zero();
    auto identity = linalg::Mat3<double>::Identity();
    auto result = zero_mat + identity;
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(i == j) {
                EXPECT_NEAR(result(i,j), 1.0, TOLERANCE);
            } else {
                EXPECT_NEAR(result(i,j), 0.0, TOLERANCE);
            }
        }
    }
}

TEST_F(NumericalPrecisionTest, OrthogonalityTests) {
    // Create orthogonal vectors
    linalg::Vec3<double> x_axis(1.0, 0.0, 0.0);
    linalg::Vec3<double> y_axis(0.0, 1.0, 0.0);
    linalg::Vec3<double> z_axis(0.0, 0.0, 1.0);
    
    // Test orthogonality via dot product
    auto dot_xy = (x_axis.transpose() * y_axis)(0,0);
    auto dot_xz = (x_axis.transpose() * z_axis)(0,0);
    auto dot_yz = (y_axis.transpose() * z_axis)(0,0);
    
    EXPECT_NEAR(dot_xy, 0.0, TOLERANCE);
    EXPECT_NEAR(dot_xz, 0.0, TOLERANCE);
    EXPECT_NEAR(dot_yz, 0.0, TOLERANCE);
    
    // Cross product should give third axis
    auto z_from_cross = x_axis.cross(y_axis);
    EXPECT_NEAR(z_from_cross(0), 0.0, TOLERANCE);
    EXPECT_NEAR(z_from_cross(1), 0.0, TOLERANCE);
    EXPECT_NEAR(z_from_cross(2), 1.0, TOLERANCE);
}

TEST_F(NumericalPrecisionTest, RotationMatrixProperties) {
    // Create a rotation matrix via quaternion
    double angle = M_PI / 4.0; // 45 degrees
    linalg::Quaternion<double> q(std::cos(angle/2), std::sin(angle/2), 0.0, 0.0);
    auto R_mat = q.matrix();
    
    // Check orthogonality: R * R^T = I
    auto RT = R_mat.transpose();
    auto should_be_identity = R_mat * RT;
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(i == j) {
                EXPECT_NEAR(should_be_identity(i,j), 1.0, TOLERANCE);
            } else {
                EXPECT_NEAR(should_be_identity(i,j), 0.0, TOLERANCE);
            }
        }
    }
    
    // Check determinant = 1
    double det = R_mat.determinant();
    EXPECT_NEAR(det, 1.0, TOLERANCE);
}

TEST_F(NumericalPrecisionTest, LieGroupProperties) {
    // Test SO(3) group properties
    linalg::Quaternion<double> q1(0.707, 0.707, 0.0, 0.0);
    linalg::Quaternion<double> q2(0.707, 0.0, 0.707, 0.0);
    
    linalg::SO3<double> R1(q1);
    linalg::SO3<double> R2(q2);
    
    // Associativity: (R1 * R2) * R1^-1 = R1 * (R2 * R1^-1)
    auto R1_inv = R1.inverse();
    auto left_side = (R1 * R2) * R1_inv;
    auto right_side = R1 * (R2 * R1_inv);
    
    auto left_mat = left_side.matrix();
    auto right_mat = right_side.matrix();
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            EXPECT_NEAR(left_mat(i,j), right_mat(i,j), TOLERANCE);
        }
    }
}

// ============================================================================
// Stress Tests with Different Data Types
// ============================================================================

TEST_F(NumericalPrecisionTest, FloatPrecision) {
    // Test with float precision
    linalg::Mat3<float> f_mat = linalg::Mat3<float>::Identity();
    f_mat(0,1) = 0.1f;
    
    auto f_inv = f_mat.inverse();
    auto f_check = f_mat * f_inv;
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(i == j) {
                EXPECT_NEAR(f_check(i,j), 1.0f, 1e-6f);
            } else {
                EXPECT_NEAR(f_check(i,j), 0.0f, 1e-6f);
            }
        }
    }
}

TEST_F(NumericalPrecisionTest, LargeDimensionMatrices) {
    // Test larger matrices
    linalg::Mat<double, 8, 8> large_mat = linalg::Mat<double, 8, 8>::Identity();
    
    // Fill with some pattern
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(i != j) {
                large_mat(i,j) = 0.01 * (i + j);
            }
        }
    }
    
    // Test matrix multiplication
    auto result = large_mat * large_mat;
    
    // Diagonal should be close to 1 + small corrections
    for(int i = 0; i < 8; i++) {
        EXPECT_GT(result(i,i), 0.9);
        EXPECT_LT(result(i,i), 1.1);
    }
}

TEST_F(NumericalPrecisionTest, VectorNormalization) {
    // Test vector normalization stability
    linalg::Vec3<double> v(3.0, 4.0, 0.0);
    double original_norm = v.norm();
    EXPECT_NEAR(original_norm, 5.0, TOLERANCE);
    
    // Normalize by dividing by norm
    auto normalized = v / original_norm;
    double normalized_norm = normalized.norm();
    EXPECT_NEAR(normalized_norm, 1.0, TOLERANCE);
    
    // Test with very small vector
    linalg::Vec3<double> tiny(1e-10, 1e-10, 1e-10);
    double tiny_norm = tiny.norm();
    if(tiny_norm > 1e-15) {  // Avoid division by zero
        auto tiny_normalized = tiny / tiny_norm;
        double tiny_normalized_norm = tiny_normalized.norm();
        EXPECT_NEAR(tiny_normalized_norm, 1.0, LOOSE_TOLERANCE);
    }
}
