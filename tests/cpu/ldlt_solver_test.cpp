// test_ldlt_eigen.cpp

#include <gtest/gtest.h>
#include <Eigen/Dense>
#include <random>

// Adjust this to the actual path of your linalg headers
#include "linalg/linalg.h"
#include "linalg/ldlt_solver.h"  // if needed, depending on your structure

namespace {

constexpr double kTol = 1e-8;

// Generic helper to run a bunch of random SPD systems for size N
template <int N>
void TestLDLTagainstEigenRandomSPD(int num_systems = 5, int num_rhs_per_system = 3)
{
    using MatN    = linalg::Mat<double, N, N>;
    using VecN    = linalg::Vec<double, N>;
    using LDLTN   = linalg::LDLT<double, N>;
    using MatrixN = Eigen::Matrix<double, N, N>;
    using VectorN = Eigen::Matrix<double, N, 1>;

    // Deterministic RNG so tests are repeatable
    std::mt19937 gen(12345 + N);
    std::normal_distribution<double> dist(0.0, 1.0);

    for (int sys = 0; sys < num_systems; ++sys)
    {
        // Build a random SPD matrix A_eig = M^T * M + alpha * I
        MatrixN M;
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                M(i, j) = dist(gen);
            }
        }

        MatrixN A_eig = M.transpose() * M;
        A_eig += 0.5 * MatrixN::Identity(); // strengthen positive-definiteness

        // Copy to your linalg::MatN
        MatN A_linalg;
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                A_linalg(i, j) = A_eig(i, j);
            }
        }

        // Factorization with your LDLT
        LDLTN solver;
        solver.compute(A_linalg);

        // Factorization with Eigen's LDLT
        Eigen::LDLT<MatrixN> ldlt(A_eig);
        ASSERT_EQ(ldlt.info(), Eigen::Success);

        // Solve several RHS with the same factorization
        for (int rhs = 0; rhs < num_rhs_per_system; ++rhs)
        {
            // Random RHS
            VectorN b_eig;
            for (int i = 0; i < N; ++i)
            {
                b_eig(i) = dist(gen);
            }

            // Copy b to your VecN
            VecN b_linalg;
            for (int i = 0; i < N; ++i)
            {
                b_linalg(i) = b_eig(i);
            }

            // Solve with your solver
            VecN x_linalg = solver.solve(b_linalg);

            // Solve with Eigen's LDLT
            VectorN x_eig = ldlt.solve(b_eig);

            // Compare component-wise
            for (int i = 0; i < N; ++i)
            {
                EXPECT_NEAR(x_eig(i), x_linalg(i), kTol)
                    << "Mismatch at size N=" << N
                    << ", system " << sys
                    << ", rhs " << rhs
                    << ", index " << i;
            }
        }
    }
}

// --- Actual tests for various sizes ---

TEST(LDLTAgainstEigen, RandomSPD_2x2)
{
    TestLDLTagainstEigenRandomSPD<2>();
}

TEST(LDLTAgainstEigen, RandomSPD_3x3)
{
    TestLDLTagainstEigenRandomSPD<3>();
}

TEST(LDLTAgainstEigen, RandomSPD_6x6)
{
    // This is the size you care about in real use.
    TestLDLTagainstEigenRandomSPD<6>();
}

TEST(LDLTAgainstEigen, RandomSPD_10x10)
{
    // A bit larger to make sure things still behave.
    TestLDLTagainstEigenRandomSPD<10>();
}

} // namespace
