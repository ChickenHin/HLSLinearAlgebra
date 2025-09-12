#include <gtest/gtest.h>
#include <cmath>
#include "linalg/linalg.h"

class ComprehensiveLinalgTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    const double TOLERANCE = 1e-10;
};

// ============================================================================
// Vector Tests - Different Sizes
// ============================================================================

TEST_F(ComprehensiveLinalgTest, VectorSizes) {
    // Test Vec2
    linalg::Vec2<double> v2(1.0, 2.0);
    EXPECT_NEAR(v2(0), 1.0, TOLERANCE);
    EXPECT_NEAR(v2(1), 2.0, TOLERANCE);
    
    // Test Vec3
    linalg::Vec3<double> v3(1.0, 2.0, 3.0);
    EXPECT_NEAR(v3(0), 1.0, TOLERANCE);
    EXPECT_NEAR(v3(1), 2.0, TOLERANCE);
    EXPECT_NEAR(v3(2), 3.0, TOLERANCE);
    
    // Test Vec4
    linalg::Vec4<double> v4(1.0, 2.0, 3.0, 4.0);
    EXPECT_NEAR(v4(0), 1.0, TOLERANCE);
    EXPECT_NEAR(v4(1), 2.0, TOLERANCE);
    EXPECT_NEAR(v4(2), 3.0, TOLERANCE);
    EXPECT_NEAR(v4(3), 4.0, TOLERANCE);
    
    // Test Vec5, Vec6, Vec8
    linalg::Vec5<double> v5(1.0, 2.0, 3.0, 4.0, 5.0);
    EXPECT_NEAR(v5(4), 5.0, TOLERANCE);
    
    linalg::Vec6<double> v6(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
    EXPECT_NEAR(v6(5), 6.0, TOLERANCE);
    
    linalg::Vec8<double> v8(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
    EXPECT_NEAR(v8(7), 8.0, TOLERANCE);
}

// ============================================================================
// Matrix Size Tests
// ============================================================================

TEST_F(ComprehensiveLinalgTest, MatrixSizes) {
    // Test Mat2
    linalg::Mat2<double> m2;
    m2(0,0) = 1.0; m2(0,1) = 2.0;
    m2(1,0) = 3.0; m2(1,1) = 4.0;
    EXPECT_NEAR(m2(1,1), 4.0, TOLERANCE);
    
    // Test Mat3
    linalg::Mat3<double> m3;
    auto identity3 = linalg::Mat3<double>::Identity();
    EXPECT_NEAR(identity3(0,0), 1.0, TOLERANCE);
    EXPECT_NEAR(identity3(1,1), 1.0, TOLERANCE);
    EXPECT_NEAR(identity3(2,2), 1.0, TOLERANCE);
    EXPECT_NEAR(identity3(0,1), 0.0, TOLERANCE);
    
    // Test Mat4
    linalg::Mat4<double> m4;
    auto identity4 = linalg::Mat4<double>::Identity();
    EXPECT_NEAR(identity4(3,3), 1.0, TOLERANCE);
    
    // Test arbitrary sized matrices
    linalg::Mat<double, 5, 3> m5x3;
    EXPECT_EQ(m5x3.rows(), 5);
    EXPECT_EQ(m5x3.cols(), 3);
    
    linalg::Mat<double, 2, 7> m2x7;
    EXPECT_EQ(m2x7.rows(), 2);
    EXPECT_EQ(m2x7.cols(), 7);
}

// ============================================================================
// Dot Product Tests
// ============================================================================

TEST_F(ComprehensiveLinalgTest, DotProduct) {
    // 2D dot product
    linalg::Vec2<double> a2(3.0, 4.0);
    linalg::Vec2<double> b2(1.0, 2.0);
    // Dot product via matrix multiplication: a^T * b
    auto dot2 = (a2.transpose() * b2)(0,0);
    EXPECT_NEAR(dot2, 11.0, TOLERANCE); // 3*1 + 4*2 = 11
    
    // 3D dot product
    linalg::Vec3<double> a3(1.0, 2.0, 3.0);
    linalg::Vec3<double> b3(4.0, 5.0, 6.0);
    auto dot3 = (a3.transpose() * b3)(0,0);
    EXPECT_NEAR(dot3, 32.0, TOLERANCE); // 1*4 + 2*5 + 3*6 = 32
    
    // 4D dot product
    linalg::Vec4<double> a4(1.0, 0.0, 1.0, 0.0);
    linalg::Vec4<double> b4(0.0, 1.0, 0.0, 1.0);
    auto dot4 = (a4.transpose() * b4)(0,0);
    EXPECT_NEAR(dot4, 0.0, TOLERANCE); // Orthogonal vectors
    
    // Higher dimensional
    linalg::Vec8<double> a8(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    linalg::Vec8<double> b8(2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0);
    auto dot8 = (a8.transpose() * b8)(0,0);
    EXPECT_NEAR(dot8, 16.0, TOLERANCE); // 8 * (1*2) = 16
}

// ============================================================================
// Cross Product Tests
// ============================================================================

TEST_F(ComprehensiveLinalgTest, CrossProduct) {
    // Basic cross product
    linalg::Vec3<double> i(1.0, 0.0, 0.0);
    linalg::Vec3<double> j(0.0, 1.0, 0.0);
    linalg::Vec3<double> k = i.cross(j);
    
    EXPECT_NEAR(k(0), 0.0, TOLERANCE);
    EXPECT_NEAR(k(1), 0.0, TOLERANCE);
    EXPECT_NEAR(k(2), 1.0, TOLERANCE);
    
    // Anti-commutative property: a × b = -(b × a)
    linalg::Vec3<double> a(1.0, 2.0, 3.0);
    linalg::Vec3<double> b(4.0, 5.0, 6.0);
    linalg::Vec3<double> axb = a.cross(b);
    linalg::Vec3<double> bxa = b.cross(a);
    
    EXPECT_NEAR(axb(0), -bxa(0), TOLERANCE);
    EXPECT_NEAR(axb(1), -bxa(1), TOLERANCE);
    EXPECT_NEAR(axb(2), -bxa(2), TOLERANCE);
    
    // Cross product with itself should be zero
    linalg::Vec3<double> v(2.0, 3.0, 4.0);
    linalg::Vec3<double> zero = v.cross(v);
    EXPECT_NEAR(zero(0), 0.0, TOLERANCE);
    EXPECT_NEAR(zero(1), 0.0, TOLERANCE);
    EXPECT_NEAR(zero(2), 0.0, TOLERANCE);
}

// ============================================================================
// Matrix Multiplication Tests
// ============================================================================

TEST_F(ComprehensiveLinalgTest, MatrixMultiplication) {
    // 2x2 * 2x2
    linalg::Mat<double, 2, 2> A;
    A(0,0) = 1; A(0,1) = 2;
    A(1,0) = 3; A(1,1) = 4;
    
    linalg::Mat<double, 2, 2> B;
    B(0,0) = 5; B(0,1) = 6;
    B(1,0) = 7; B(1,1) = 8;
    
    auto C = A * B;
    EXPECT_NEAR(C(0,0), 19.0, TOLERANCE); // 1*5 + 2*7
    EXPECT_NEAR(C(0,1), 22.0, TOLERANCE); // 1*6 + 2*8
    EXPECT_NEAR(C(1,0), 43.0, TOLERANCE); // 3*5 + 4*7
    EXPECT_NEAR(C(1,1), 50.0, TOLERANCE); // 3*6 + 4*8
    
    // 3x2 * 2x4
    linalg::Mat<double, 3, 2> D;
    D(0,0) = 1; D(0,1) = 2;
    D(1,0) = 3; D(1,1) = 4;
    D(2,0) = 5; D(2,1) = 6;
    
    linalg::Mat<double, 2, 4> E;
    E(0,0) = 1; E(0,1) = 0; E(0,2) = 1; E(0,3) = 0;
    E(1,0) = 0; E(1,1) = 1; E(1,2) = 0; E(1,3) = 1;
    
    auto F = D * E; // Should be 3x4
    EXPECT_EQ(F.rows(), 3);
    EXPECT_EQ(F.cols(), 4);
    EXPECT_NEAR(F(0,0), 1.0, TOLERANCE); // 1*1 + 2*0
    EXPECT_NEAR(F(0,1), 2.0, TOLERANCE); // 1*0 + 2*1
    
    // Matrix-vector multiplication
    linalg::Vec3<double> v(1.0, 2.0, 3.0);
    auto identity = linalg::Mat3<double>::Identity();
    auto result = identity * v;
    EXPECT_NEAR(result(0), 1.0, TOLERANCE);
    EXPECT_NEAR(result(1), 2.0, TOLERANCE);
    EXPECT_NEAR(result(2), 3.0, TOLERANCE);
}
