#include <gtest/gtest.h>
#include <limits>
#include <cmath>
#include "linalg/linalg.h"
#include "linalg/ldlt_solver.h"

class LinalgAdditionalTest : public ::testing::Test
{
protected:
    const double T = 1e-10;
};

TEST_F(LinalgAdditionalTest, ZeroAndIdentity)
{
    auto Z = linalg::Mat3<double>::Zero();
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            EXPECT_NEAR(Z(r, c), 0.0, T);

    auto I = linalg::Mat4<double>::Identity();
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            EXPECT_NEAR(I(i, j), i == j ? 1.0 : 0.0, T);
}

TEST_F(LinalgAdditionalTest, ElementAccessAndDataLayout)
{
    // Column-major storage is used. Validate basic access pattern.
    linalg::Mat<double, 2, 3> M; // 2 rows, 3 cols
    // Fill by (r,c)
    M(0, 0) = 1;
    M(1, 0) = 2; // col 0
    M(0, 1) = 3;
    M(1, 1) = 4; // col 1
    M(0, 2) = 5;
    M(1, 2) = 6; // col 2

    EXPECT_NEAR(M(0, 0), 1.0, T);
    EXPECT_NEAR(M(1, 0), 2.0, T);
    EXPECT_NEAR(M(0, 1), 3.0, T);
    EXPECT_NEAR(M(1, 1), 4.0, T);
    EXPECT_NEAR(M(0, 2), 5.0, T);
    EXPECT_NEAR(M(1, 2), 6.0, T);
}

TEST_F(LinalgAdditionalTest, MatrixArithmetic)
{
    linalg::Mat2<double> A; // default zeros
    A(0, 0) = 1;
    A(0, 1) = 2;
    A(1, 0) = 3;
    A(1, 1) = 4;

    linalg::Mat2<double> B;
    B(0, 0) = 5;
    B(0, 1) = 6;
    B(1, 0) = 7;
    B(1, 1) = 8;

    auto C = A + B;
    EXPECT_NEAR(C(0, 0), 6.0, T);
    EXPECT_NEAR(C(0, 1), 8.0, T);
    EXPECT_NEAR(C(1, 0), 10.0, T);
    EXPECT_NEAR(C(1, 1), 12.0, T);

    auto D = B - A;
    EXPECT_NEAR(D(0, 0), 4.0, T);
    EXPECT_NEAR(D(0, 1), 4.0, T);
    EXPECT_NEAR(D(1, 0), 4.0, T);
    EXPECT_NEAR(D(1, 1), 4.0, T);

    auto N = -A;
    EXPECT_NEAR(N(0, 0), -1.0, T);
    EXPECT_NEAR(N(0, 1), -2.0, T);
    EXPECT_NEAR(N(1, 0), -3.0, T);
    EXPECT_NEAR(N(1, 1), -4.0, T);
}

TEST_F(LinalgAdditionalTest, ScalarMultiplyDivide)
{
    linalg::Mat<double, 2, 2> M;
    M(0, 0) = 1;
    M(0, 1) = 2;
    M(1, 0) = 3;
    M(1, 1) = 4;

    auto S = 2.0 * M;
    EXPECT_NEAR(S(0, 0), 2.0, T);
    EXPECT_NEAR(S(0, 1), 4.0, T);
    EXPECT_NEAR(S(1, 0), 6.0, T);
    EXPECT_NEAR(S(1, 1), 8.0, T);

    auto Q = M / 2.0;
    EXPECT_NEAR(Q(0, 0), 0.5, T);
    EXPECT_NEAR(Q(0, 1), 1.0, T);
    EXPECT_NEAR(Q(1, 0), 1.5, T);
    EXPECT_NEAR(Q(1, 1), 2.0, T);
}

TEST_F(LinalgAdditionalTest, ConvHelper)
{
    // conv accumulates elementwise product into OutType.
    linalg::Mat<double, 2, 2> A;
    A(0, 0) = 1;
    A(0, 1) = 2;
    A(1, 0) = 3;
    A(1, 1) = 4;

    linalg::Mat<double, 2, 2> B;
    B(0, 0) = 5;
    B(0, 1) = 6;
    B(1, 0) = 7;
    B(1, 1) = 8;

    double sum = A.conv<double, double>(B); // 1*5 + 2*6 + 3*7 + 4*8 = 70
    EXPECT_NEAR(sum, 70.0, T);
}

TEST_F(LinalgAdditionalTest, Mat3DeterminantAndInverseRobust)
{
    linalg::Mat3<double> M;
    M(0, 0) = 4;
    M(0, 1) = 7;
    M(0, 2) = 2;
    M(1, 0) = 3;
    M(1, 1) = 6;
    M(1, 2) = 1;
    M(2, 0) = 2;
    M(2, 1) = 5;
    M(2, 2) = 1;

    double det = M.determinant();
    EXPECT_NEAR(det, 3.0, 1e-12);

    auto Minv = M.inverse();
    auto I = M * Minv;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            EXPECT_NEAR(I(i, j), i == j ? 1.0 : 0.0, 1e-9);
        }
    }
}

TEST_F(LinalgAdditionalTest, OuterProductAndWedge)
{
    linalg::Vec3<double> a(1.0, 2.0, 3.0);
    linalg::Vec3<double> b(4.0, 5.0, 6.0);

    auto op = linalg::outerProduct(a, b);
    EXPECT_NEAR(op(0, 0), 4.0, T);
    EXPECT_NEAR(op(2, 1), 3.0 * 5.0, T);

    auto W = linalg::wedge(a);
    EXPECT_NEAR(W(0, 1), -a(2), T);
    EXPECT_NEAR(W(0, 2), a(1), T);
    EXPECT_NEAR(W(1, 0), a(2), T);
    EXPECT_NEAR(W(1, 2), -a(0), T);
    EXPECT_NEAR(W(2, 0), -a(1), T);
    EXPECT_NEAR(W(2, 1), a(0), T);
}

TEST_F(LinalgAdditionalTest, SO3LeftJacobianConsistencySmallAngle)
{
    linalg::Vec3<double> phi(1e-9, 2e-9, -1e-9);
    auto J = linalg::so3LeftJacobian(phi);
    // For very small phi, J ~ I + 0.5 * [phi]_x; here we check diagonals near 1.
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_NEAR(J(i, i), 1.0, 1e-9);
    }
}

TEST_F(LinalgAdditionalTest, SO3FromMatrixConstructor)
{
    // 90deg around Z
    double c = std::cos(M_PI / 2), s = std::sin(M_PI / 2);
    linalg::Mat3<double> Rm;
    Rm(0, 0) = c;
    Rm(0, 1) = -s;
    Rm(0, 2) = 0;
    Rm(1, 0) = s;
    Rm(1, 1) = c;
    Rm(1, 2) = 0;
    Rm(2, 0) = 0;
    Rm(2, 1) = 0;
    Rm(2, 2) = 1;

    linalg::SO3<double> R(Rm);
    linalg::Vec3<double> v(1.0, 0.0, 0.0);
    auto rv = R * v;
    EXPECT_NEAR(rv(0), 0.0, 1e-9);
    EXPECT_NEAR(rv(1), 1.0, 1e-9);
    EXPECT_NEAR(rv(2), 0.0, 1e-9);
}

TEST_F(LinalgAdditionalTest, QuaternionVectorRotation)
{
    // 90deg about Z: q = [cos(theta/2), 0, 0, sin(theta/2)]
    double th = M_PI / 2;
    linalg::Quaternion<double> q(std::cos(th / 2), 0.0, 0.0, std::sin(th / 2));
    linalg::Vec3<double> y(0.0, 1.0, 0.0);
    auto x = q * y; // rotate (0,1,0) by +90deg about Z => (-1,0,0)
    EXPECT_NEAR(x(0), -1.0, 1e-9);
    EXPECT_NEAR(x(1), 0.0, 1e-9);
    EXPECT_NEAR(x(2), 0.0, 1e-9);
}

TEST_F(LinalgAdditionalTest, SE3ExpConsistency)
{
    // xi = (rho, phi). For small phi, translation ~ (I + 0.5 [phi]_x) * rho
    linalg::Vec3<double> rho(1.0, 2.0, 3.0);
    linalg::Vec3<double> phi(1e-6, -2e-6, 3e-6);
    linalg::Vec6<double> xi(rho(0), rho(1), rho(2), phi(0), phi(1), phi(2));

    auto Texp = linalg::SE3<double>::exp(xi);
    auto Tm = Texp.matrix();

    // Extract translation from matrix last row of your format (row 3, col 0..2)
    linalg::Mat3<double> J = linalg::so3LeftJacobian(phi);
    auto Jrho = J * rho;

    EXPECT_NEAR(Tm(0, 3), Jrho(0), 1e-6);
    EXPECT_NEAR(Tm(1, 3), Jrho(1), 1e-6);
    EXPECT_NEAR(Tm(2, 3), Jrho(2), 1e-6);
}

TEST_F(LinalgAdditionalTest, VectorHelpersAndEquality)
{
    linalg::Vec3<double> a(1.0, 2.0, 3.0), b(1.0, 2.0, 3.0);
    EXPECT_TRUE(a == b);

    linalg::Vec4<double> v4(1.0, 2.0, 3.0, 4.0);
    auto xy = v4.xy();
    EXPECT_NEAR(xy(0), 1.0, T);
    EXPECT_NEAR(xy(1), 2.0, T);
}

TEST_F(LinalgAdditionalTest, LDLTResidualCheck)
{
    using Type = double;
    constexpr int N = 3;
    // Build SPD matrix A = M^T*M + alpha*I
    linalg::Mat<Type, N, N> M;
    M(0, 0) = 1;
    M(0, 1) = 2;
    M(0, 2) = -1;
    M(1, 0) = 0;
    M(1, 1) = 1;
    M(1, 2) = 1;
    M(2, 0) = 2;
    M(2, 1) = 0;
    M(2, 2) = 1;
    auto Mt = M.transpose();
    auto A = Mt * M; // SPD
    for (int i = 0; i < N; ++i)
        A(i, i) += 1e-6; // improve conditioning

    linalg::Mat<Type, N, 1> b;
    b(0, 0) = 1.0;
    b(1, 0) = -2.0;
    b(2, 0) = 3.0;

    linalg::LDLT<Type, N> solver;
    solver.compute(A);
    auto x = solver.solve(b);

    // residual r = A x - b
    auto r = A * x - b;
    Type rnorm = r.norm();
    EXPECT_LT(rnorm, 1e-9);
}

TEST_F(LinalgAdditionalTest, NearSingularInverseStability)
{
    // Ill-conditioned but invertible diagonal matrix
    linalg::Mat3<double> A = linalg::Mat3<double>::Zero();
    A(0, 0) = 1e-8;
    A(1, 1) = 1.0;
    A(2, 2) = 2.0;

    auto Ainverse = A.inverse();
    auto I = A * Ainverse;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            EXPECT_NEAR(I(i, j), i == j ? 1.0 : 0.0, 1e-9);
        }
    }
}

TEST_F(LinalgAdditionalTest, SO3ExpAtPiAxes)
{
    const double pi = M_PI;
    // Rotation by pi about X
    linalg::Vec3<double> phix(pi, 0.0, 0.0);
    auto Rx = linalg::SO3<double>::exp(phix).matrix();
    EXPECT_NEAR(Rx(0, 0), 1.0, 1e-9);
    EXPECT_NEAR(Rx(1, 1), -1.0, 1e-9);
    EXPECT_NEAR(Rx(2, 2), -1.0, 1e-9);
    EXPECT_NEAR(Rx(0, 1), 0.0, 1e-9);
    EXPECT_NEAR(Rx(0, 2), 0.0, 1e-9);

    // Rotation by pi about Y
    linalg::Vec3<double> phiy(0.0, pi, 0.0);
    auto Ry = linalg::SO3<double>::exp(phiy).matrix();
    EXPECT_NEAR(Ry(1, 1), 1.0, 1e-9);
    EXPECT_NEAR(Ry(0, 0), -1.0, 1e-9);
    EXPECT_NEAR(Ry(2, 2), -1.0, 1e-9);

    // Rotation by pi about Z
    linalg::Vec3<double> phiz(0.0, 0.0, pi);
    auto Rz = linalg::SO3<double>::exp(phiz).matrix();
    EXPECT_NEAR(Rz(2, 2), 1.0, 1e-9);
    EXPECT_NEAR(Rz(0, 0), -1.0, 1e-9);
    EXPECT_NEAR(Rz(1, 1), -1.0, 1e-9);
}

TEST_F(LinalgAdditionalTest, SO3LeftJacobianModerateAngle)
{
    // Validate J against its closed form for axis z, theta = 0.5
    double theta = 0.5;
    linalg::Vec3<double> phi(0.0, 0.0, theta);
    auto J = linalg::so3LeftJacobian(phi);

    // Recompute expected from closed form with axis = [0 0 1]
    double s = std::sin(theta);
    double c = std::cos(theta);
    linalg::Mat3<double> I = linalg::Mat3<double>::Identity();
    linalg::Vec3<double> axis(0.0, 0.0, 1.0);
    auto aaT = linalg::outerProduct(axis, axis);
    auto K = linalg::wedge(axis);
    auto Jexp = I * (s / theta) + aaT * (1.0 - s / theta) + K * ((1.0 - c) / theta);

    for (int r = 0; r < 3; ++r)
    {
        for (int c2 = 0; c2 < 3; ++c2)
        {
            EXPECT_NEAR(J(r, c2), Jexp(r, c2), 1e-12);
        }
    }
}
