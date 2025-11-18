#pragma once

#include "linalg/linalg.h"

namespace linalg
{
    template <typename Type, int size>
    class LDLT
    {
    public:
        using MatN = Mat<Type, size, size>;
        using VecN = VecC<Type, size>;

        LDLT() {}

        // Compute the LDLT decomposition of a matrix A.
        // This must be called before solve().
        void compute(const MatN &_A)
        {
            A_ = _A;
            ldlt_decompose();
        }

        // Solve A x = b for x, given b.
        // Assumes compute() has been called.
        VecN solve(const VecN &b)
        {
            // 1) Solve L y = b (Forward substitution)
            VecN y;
            forward_substitution(L_, b, y);

            // 3) Solve D z = y
            VecN z;
            diagonal_solve(D_, y, z);

            // 4) Solve L^T x = z
            VecN x;
            back_substitution_transpose(L_, z, x);

            return x;
        }

    private:
        void ldlt_decompose()
        {
            // Initialize L to identity and D to zero.
            L_ = MatN::Identity();
            D_ = VecN::Zero();

            for (int i = 0; i < size; ++i)
            {
                // 1) Compute D[i] = A[i][i] - sum_{k=0 to i-1}(L[i][k]^2 * D[k])
                Type sum = A_(i, i);
                for (int k = 0; k < i; ++k)
                {
                    sum -= L_(i, k) * L_(i, k) * D_(k);
                }
                D_(i) = sum;

                if (math::fabs(D_(i)) < Type(1e-12))
                {
                    // Matrix is not positive definite or is singular.
                    // A robust implementation would throw or return an error.
                }

                // 2) Compute L[j][i] for j = i+1..n-1
                Type inv_Di = Type(1) / D_(i);
                for (int j = i + 1; j < size; ++j)
                {
                    Type val = A_(j, i);
                    // Subtract the part contributed by previous columns
                    for (int k = 0; k < i; ++k)
                    {
                        val -= L_(j, k) * L_(i, k) * D_(k);
                    }
                    // L[j][i] = (A[j][i] - ...) / D[i]
                    L_(j, i) = val * inv_Di;
                }
            }
        }

        // Forward substitution for L y = b
        // L is lower triangular with diagonal = 1.0
        void forward_substitution(const MatN &L, const VecN &b, VecN &y)
        {
            for (int i = 0; i < size; ++i)
            {
                Type sum = b(i);
                for (int j = 0; j < i; ++j)
                {
                    sum -= L(i, j) * y(j);
                }
                // L(i, i) is 1.0
                y(i) = sum;
            }
        }

        // Diagonal solve for D z = y
        // D is diagonal, stored as a vector. z[i] = y[i] / D[i]
        void diagonal_solve(const VecN &D, const VecN &y, VecN &z)
        {
            for (int i = 0; i < size; ++i)
            {
                z(i) = y(i) / D(i);
            }
        }

        // Back substitution for L^T x = z
        // L is lower-triangular, so L^T is upper-triangular.
        void back_substitution_transpose(const MatN &L, const VecN &z, VecN &x)
        {
            for (int i = size - 1; i >= 0; --i)
            {
                Type sum = z(i);
                for (int j = i + 1; j < size; ++j)
                {
                    sum -= L(j, i) * x(j); // L^T[i][j] = L[j][i]
                }
                // L[i][i] = 1.0
                x(i) = sum;
            }
        }

        MatN A_;
        MatN L_;
        VecN D_; // Store diagonal of D as a vector
    };

    template <typename Type>
    class LDLTx
    {
    public:
        LDLTx(int size)
            : size_(size), A_(size, size), L_(size, size), D_(size)
        {
        }

        // Compute the LDLT decomposition of a matrix A.
        // This must be called before solve().
        void compute(const Matx<Type> &_A)
        {
            A_ = _A;
            ldlt_decompose();
        }

        // Solve A x = b for x, given b.
        // Assumes compute() has been called.
        VecxC<Type> solve(const VecxC<Type> &b)
        {
            // 1) Solve L y = b (Forward substitution)
            Matx<Type> y;
            forward_substitution(L_, b, y);

            // 3) Solve D z = y
            Matx<Type> z;
            diagonal_solve(D_, y, z);

            // 4) Solve L^T x = z
            Matx<Type> x;
            back_substitution_transpose(L_, z, x);

            return x;
        }

    private:
        void ldlt_decompose()
        {
            // Initialize L to identity and D to zero.
            L_ = Matx<Type>::Identity(size_, size_);
            D_ = VecxC<Type>::Zero(size_);

            for (int i = 0; i < size_; ++i)
            {
                // 1) Compute D[i] = A[i][i] - sum_{k=0 to i-1}(L[i][k]^2 * D[k])
                Type sum = A_(i, i);
                for (int k = 0; k < i; ++k)
                {
                    sum -= L_(i, k) * L_(i, k) * D_(k);
                }
                D_(i) = sum;

                if (math::fabs(D_(i)) < Type(1e-12))
                {
                    // Matrix is not positive definite or is singular.
                    // A robust implementation would throw or return an error.
                }

                // 2) Compute L[j][i] for j = i+1..n-1
                Type inv_Di = Type(1) / D_(i);
                for (int j = i + 1; j < size_; ++j)
                {
                    Type val = A_(j, i);
                    // Subtract the part contributed by previous columns
                    for (int k = 0; k < i; ++k)
                    {
                        val -= L_(j, k) * L_(i, k) * D_(k);
                    }
                    // L[j][i] = (A[j][i] - ...) / D[i]
                    L_(j, i) = val * inv_Di;
                }
            }
        }

        // Forward substitution for L y = b
        // L is lower triangular with diagonal = 1.0
        void forward_substitution(const Matx<Type> &L, const VecxC<Type> &b, VecxC<Type> &y)
        {
            for (int i = 0; i < size_; ++i)
            {
                Type sum = b(i);
                for (int j = 0; j < i; ++j)
                {
                    sum -= L(i, j) * y(j);
                }
                // L(i, i) is 1.0
                y(i) = sum;
            }
        }

        // Diagonal solve for D z = y
        // D is diagonal, stored as a vector. z[i] = y[i] / D[i]
        void diagonal_solve(const VecxC<Type> &D, const VecxC<Type> &y, VecxC<Type> &z)
        {
            for (int i = 0; i < size_; ++i)
            {
                z(i) = y(i) / D(i);
            }
        }

        // Back substitution for L^T x = z
        // L is lower-triangular, so L^T is upper-triangular.
        void back_substitution_transpose(const Matx<Type> &L, const VecxC<Type> &z, VecxC<Type> &x)
        {
            for (int i = size_ - 1; i >= 0; --i)
            {
                Type sum = z(i);
                for (int j = i + 1; j < size_; ++j)
                {
                    sum -= L(j, i) * x(j); // L^T[i][j] = L[j][i]
                }
                // L[i][i] = 1.0
                x(i) = sum;
            }
        }

        Matx<Type> A_;
        Matx<Type> L_;
        VecxC<Type> D_; // Store diagonal of D as a vector
        int size_;
    };
}
