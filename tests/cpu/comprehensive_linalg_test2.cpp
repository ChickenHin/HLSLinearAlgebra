#include <gtest/gtest.h>
#include <cmath>
#include "linalg/linalg.h"

class ComprehensiveLinalgTest2 : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    const double TOLERANCE = 1e-10;
};

// ============================================================================
// Matrix Operations Tests (Transpose, Inverse, Determinant)
// ============================================================================

TEST_F(ComprehensiveLinalgTest2, MatrixOperations) {
    // Transpose test
    linalg::Mat<double, 2, 3> A;
    A(0,0) = 1; A(0,1) = 2; A(0,2) = 3;
    A(1,0) = 4; A(1,1) = 5; A(1,2) = 6;
    
    auto AT = A.transpose();
    EXPECT_EQ(AT.rows(), 3);
    EXPECT_EQ(AT.cols(), 2);
    EXPECT_NEAR(AT(0,0), 1.0, TOLERANCE);
    EXPECT_NEAR(AT(1,0), 2.0, TOLERANCE);
    EXPECT_NEAR(AT(2,0), 3.0, TOLERANCE);
    EXPECT_NEAR(AT(0,1), 4.0, TOLERANCE);
    EXPECT_NEAR(AT(1,1), 5.0, TOLERANCE);
    EXPECT_NEAR(AT(2,1), 6.0, TOLERANCE);
    
    // 3x3 Determinant test
    linalg::Mat3<double> B;
    B(0,0) = 1; B(0,1) = 2; B(0,2) = 3;
    B(1,0) = 0; B(1,1) = 1; B(1,2) = 4;
    B(2,0) = 5; B(2,1) = 6; B(2,2) = 0;
    
    double det = B.determinant();
    EXPECT_NEAR(det, 1.0, TOLERANCE); // Manual calculation: 1*(1*0-4*6) - 2*(0*0-4*5) + 3*(0*6-1*5) = -24 + 40 - 15 = 1
    
    // 3x3 Inverse test
    linalg::Mat3<double> C = linalg::Mat3<double>::Identity();
    C(0,1) = 2.0; // Make it [1 2 0; 0 1 0; 0 0 1]
    
    auto C_inv = C.inverse();
    auto should_be_identity = C * C_inv;
    
    // Check if C * C_inv = I
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(i == j) {
                EXPECT_NEAR(should_be_identity(i,j), 1.0, TOLERANCE);
            } else {
                EXPECT_NEAR(should_be_identity(i,j), 0.0, TOLERANCE);
            }
        }
    }
}

// ============================================================================
// Vector Norms Tests
// ============================================================================

TEST_F(ComprehensiveLinalgTest2, VectorNorms) {
    // 2D norm
    linalg::Vec2<double> v2(3.0, 4.0);
    EXPECT_NEAR(v2.norm(), 5.0, TOLERANCE); // sqrt(9 + 16) = 5
    
    // 3D norm
    linalg::Vec3<double> v3(1.0, 2.0, 2.0);
    EXPECT_NEAR(v3.norm(), 3.0, TOLERANCE); // sqrt(1 + 4 + 4) = 3
    
    // Unit vector norm
    linalg::Vec3<double> unit(1.0, 0.0, 0.0);
    EXPECT_NEAR(unit.norm(), 1.0, TOLERANCE);
    
    // Zero vector norm
    linalg::Vec4<double> zero = linalg::Vec4<double>();
    EXPECT_NEAR(zero.norm(), 0.0, TOLERANCE);
}

// ============================================================================
// Quaternion Tests
// ============================================================================

TEST_F(ComprehensiveLinalgTest2, QuaternionOperations) {
    // Identity quaternion
    linalg::Quaternion<double> q_identity(1.0, 0.0, 0.0, 0.0);
    auto identity_matrix = q_identity.matrix();
    auto expected_identity = linalg::Mat3<double>::Identity();
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            EXPECT_NEAR(identity_matrix(i,j), expected_identity(i,j), TOLERANCE);
        }
    }
    
    // Quaternion multiplication (identity * q = q)
    linalg::Quaternion<double> q(0.5, 0.5, 0.5, 0.5);
    auto result = q_identity * q;
    EXPECT_NEAR(result.w_, q.w_, TOLERANCE);
    EXPECT_NEAR(result.x_, q.x_, TOLERANCE);
    EXPECT_NEAR(result.y_, q.y_, TOLERANCE);
    EXPECT_NEAR(result.z_, q.z_, TOLERANCE);
    
    // Quaternion inverse
    auto q_inv = q.inverse();
    auto should_be_identity = q * q_inv;
    EXPECT_NEAR(should_be_identity.w_, 1.0, TOLERANCE);
    EXPECT_NEAR(should_be_identity.x_, 0.0, TOLERANCE);
    EXPECT_NEAR(should_be_identity.y_, 0.0, TOLERANCE);
    EXPECT_NEAR(should_be_identity.z_, 0.0, TOLERANCE);
    
    // Vector rotation by quaternion
    linalg::Vec3<double> v(1.0, 0.0, 0.0);
    auto rotated = q_identity * v; // Identity should not change the vector
    EXPECT_NEAR(rotated(0), 1.0, TOLERANCE);
    EXPECT_NEAR(rotated(1), 0.0, TOLERANCE);
    EXPECT_NEAR(rotated(2), 0.0, TOLERANCE);
}

// ============================================================================
// SO(3) Rotation Group Tests
// ============================================================================

TEST_F(ComprehensiveLinalgTest2, SO3Operations) {
    // Identity rotation
    linalg::SO3<double> R_identity;
    auto identity_mat = R_identity.matrix();
    auto expected = linalg::Mat3<double>::Identity();
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            EXPECT_NEAR(identity_mat(i,j), expected(i,j), TOLERANCE);
        }
    }
    
    // Rotation composition (R * I = R)
    linalg::Quaternion<double> q(0.707, 0.0, 0.0, 0.707); // 90° rotation around x-axis
    linalg::SO3<double> R(q);
    auto composed = R * R_identity;
    auto composed_mat = composed.matrix();
    auto R_mat = R.matrix();
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            EXPECT_NEAR(composed_mat(i,j), R_mat(i,j), TOLERANCE);
        }
    }
    
    // Rotation inverse (R * R^-1 = I)
    auto R_inv = R.inverse();
    auto should_be_identity = R * R_inv;
    auto identity_check = should_be_identity.matrix();
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(i == j) {
                EXPECT_NEAR(identity_check(i,j), 1.0, TOLERANCE);
            } else {
                EXPECT_NEAR(identity_check(i,j), 0.0, TOLERANCE);
            }
        }
    }
    
    // Vector rotation
    linalg::Vec3<double> v(1.0, 0.0, 0.0);
    auto rotated = R_identity * v; // Identity rotation
    EXPECT_NEAR(rotated(0), 1.0, TOLERANCE);
    EXPECT_NEAR(rotated(1), 0.0, TOLERANCE);
    EXPECT_NEAR(rotated(2), 0.0, TOLERANCE);
}

// ============================================================================
// SE(3) Special Euclidean Group Tests
// ============================================================================

TEST_F(ComprehensiveLinalgTest2, SE3Operations) {
    // Identity transformation
    linalg::SE3<double> T_identity;
    auto identity_mat = T_identity.matrix();
    
    // Check 4x4 identity matrix structure
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(i == j) {
                EXPECT_NEAR(identity_mat(i,j), 1.0, TOLERANCE);
            } else {
                EXPECT_NEAR(identity_mat(i,j), 0.0, TOLERANCE);
            }
        }
    }
    
    // SE(3) with translation
    linalg::SO3<double> R;
    linalg::Vec3<double> t(1.0, 2.0, 3.0);
    linalg::SE3<double> T(R, t);
    
    auto T_mat = T.matrix();
    // Check translation part
    EXPECT_NEAR(T_mat(3,0), 1.0, TOLERANCE);
    EXPECT_NEAR(T_mat(3,1), 2.0, TOLERANCE);
    EXPECT_NEAR(T_mat(3,2), 3.0, TOLERANCE);
    EXPECT_NEAR(T_mat(3,3), 1.0, TOLERANCE);
    
    // Point transformation
    linalg::Vec3<double> p(0.0, 0.0, 0.0);
    auto transformed = T * p;
    EXPECT_NEAR(transformed(0), 1.0, TOLERANCE);
    EXPECT_NEAR(transformed(1), 2.0, TOLERANCE);
    EXPECT_NEAR(transformed(2), 3.0, TOLERANCE);
    
    // SE(3) composition
    linalg::Vec3<double> t2(1.0, 1.0, 1.0);
    linalg::SE3<double> T2(R, t2);
    auto T_composed = T * T2;
    
    // Transform origin twice
    linalg::Vec3<double> origin(0.0, 0.0, 0.0);
    auto result1 = T * (T2 * origin);
    auto result2 = T_composed * origin;
    
    EXPECT_NEAR(result1(0), result2(0), TOLERANCE);
    EXPECT_NEAR(result1(1), result2(1), TOLERANCE);
    EXPECT_NEAR(result1(2), result2(2), TOLERANCE);
    
    // SE(3) inverse
    auto T_inv = T.inverse();
    auto should_be_identity = T * T_inv;
    auto check_identity = should_be_identity.matrix();
    
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(i == j) {
                EXPECT_NEAR(check_identity(i,j), 1.0, TOLERANCE);
            } else {
                EXPECT_NEAR(check_identity(i,j), 0.0, TOLERANCE);
            }
        }
    }
}

// ============================================================================
// Exponential Map Tests
// ============================================================================

TEST_F(ComprehensiveLinalgTest2, ExponentialMaps) {
    // so(3) exponential map - zero vector should give identity
    linalg::Vec3<double> zero_phi(0.0, 0.0, 0.0);
    auto R_from_zero = linalg::so3Exp(zero_phi);
    auto R_zero_mat = R_from_zero.matrix();
    auto identity = linalg::Mat3<double>::Identity();
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            EXPECT_NEAR(R_zero_mat(i,j), identity(i,j), TOLERANCE);
        }
    }
    
    // Small angle approximation
    linalg::Vec3<double> small_phi(1e-8, 0.0, 0.0);
    auto R_small = linalg::so3Exp(small_phi);
    auto R_small_mat = R_small.matrix();
    
    // Should be approximately identity + skew(phi)
    EXPECT_NEAR(R_small_mat(0,0), 1.0, TOLERANCE);
    EXPECT_NEAR(R_small_mat(1,1), 1.0, TOLERANCE);
    EXPECT_NEAR(R_small_mat(2,2), 1.0, TOLERANCE);
    
    // se(3) exponential map
    linalg::Vec6<double> zero_xi(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    auto T_from_zero = linalg::se3Exp(zero_xi);
    auto T_zero_mat = T_from_zero.matrix();
    
    // Should be 4x4 identity
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(i == j) {
                EXPECT_NEAR(T_zero_mat(i,j), 1.0, TOLERANCE);
            } else {
                EXPECT_NEAR(T_zero_mat(i,j), 0.0, TOLERANCE);
            }
        }
    }
}
