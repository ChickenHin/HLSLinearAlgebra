
#pragma once

#include <cassert> // for assert
#include <memory>  // for unique_ptr

// #include <cmath>

namespace linalg
{
    //============================================================
    // Basic fixed-size matrix class
    //============================================================

    template <typename Type>
    class Matx
    {
    public:
        // Matx()
        //     : data_(nullptr), rows_(0), cols_(0)
        //{
        // }

        Matx(int rows, int cols)
            : data_(rows * cols > 0 ? std::make_unique<Type[]>(rows * cols) : nullptr), rows_(rows), cols_(cols)
        {
        }

        Matx(const Matx &other)
            : Matx(other.rows_, other.cols_)
        {
            if (data_)
            {
                std::copy(other.data_.get(),
                          other.data_.get() + rows_ * cols_,
                          data_.get());
            }
        }

        Matx &operator=(const Matx &other)
        {
            if (this == &other)
                return *this;

            // Reallocate if size changes
            int newSize = other.rows_ * other.cols_;
            if (newSize != rows_ * cols_)
            {
                data_.reset();
                if (newSize > 0)
                {
                    data_ = std::make_unique<Type[]>(newSize);
                }
            }

            rows_ = other.rows_;
            cols_ = other.cols_;

            if (data_)
            {
                std::copy(other.data_.get(),
                          other.data_.get() + rows_ * cols_,
                          data_.get());
            }

            return *this;
        }

        // Matx(const Matx &) = delete;
        // Matx &operator=(const Matx &) = delete;

        void setZero()
        {
            assert(rows_ * cols_ > 0);

            for (int i = 0; i < rows_ * cols_; i++)
            {
                data_[i] = Type(0);
            }
        }

        void setIdentity()
        {
            assert(rows_ == cols_);
            assert(rows_ * cols_ > 0);

            for (int i = 0; i < cols_; i++)
                data_[i * rows_ + i] = Type(1);
        }

        static Matx Zero(int rows, int cols)
        {
            Matx result(rows, cols);
            for (int i = 0; i < rows * cols; i++)
                result.data_[i] = Type(0);
            return result;
        }

        static Matx Identity(int rows, int cols)
        {
            assert(rows == cols);

            Matx result = Zero(rows, cols);
            for (int i = 0; i < rows; i++)
                result(i, i) = Type(1);
            return result;
        }

        Matx<Type> transpose() const
        {
            Matx<Type> result = Zero(cols_, rows_);
            for (int r = 0; r < rows_; r++)
                for (int c = 0; c < cols_; c++)
                    result(c, r) = get_(r, c);
            return result;
        }

        template <typename Type2>
        Matx<Type> operator*(const Matx<Type2> &rhs) const
        {
            assert(cols_ == rhs.rows());

            Matx<Type> result = Matx<Type>::Zero(rows_, rhs.cols());
            for (int r = 0; r < rows_; r++)
            {
                for (int c = 0; c < rhs.cols(); c++)
                {
                    Type acc = 0.0;
                    for (int k = 0; k < cols_; k++)
                    {
                        acc += get_(r, k) * rhs(k, c);
                    }
                    result(r, c) = acc;
                }
            }
            return result;
        }

        Matx operator+(const Matx &other) const
        {
            assert(rows_ == other.rows() && cols_ == other.cols());

            Matx result(rows_, cols_);
            for (int c = 0; c < cols_; c++)
                for (int r = 0; r < rows_; r++)
                    result(r, c) = get_(r, c) + other(r, c);
            return result;
        }

        Matx operator-(const Matx &other) const
        {
            assert(rows_ == other.rows() && cols_ == other.cols());

            Matx result(rows_, cols_);
            for (int c = 0; c < cols_; c++)
                for (int r = 0; r < rows_; r++)
                    result(r, c) = get_(r, c) - other(r, c);
            return result;
        }

        template <typename OutType, typename InType>
        OutType conv(const Matx<InType> &rhs) const
        {
            OutType result = Type(0);
            for (int c = 0; c < cols_; c++)
                for (int r = 0; r < rows_; r++)
                    result += OutType(get_(r, c) * rhs(r, c));
            return result;
        }

        template <typename Type2>
        Type dot(const Matx<Type2> &rhs)
        {
            assert(rows_ == rhs.rows() && cols_ == rhs.cols());

            Type result = Type(0);
            for (int c = 0; c < cols_; c++)
                for (int r = 0; r < rows_; r++)
                    result += get_(r, c) * rhs(r, c);
            return result;
        }

        Matx operator+=(const Matx &other)
        {
            assert(rows_ == other.rows() && cols_ == other.cols());

            Matx result(rows_, cols_);
            for (int c = 0; c < cols_; c++)
                for (int r = 0; r < rows_; r++)
                    result(r, c) = get_(r, c) + other(r, c);
            return result;
        }

        template <typename Type2>
        Matx operator*=(const Type2 &s)
        {
            Matx result(rows_, cols_);
            for (int c = 0; c < cols_; c++)
                for (int r = 0; r < rows_; r++)
                    result(r, c) = get_(r, c) * s;
            return result;
        }

        Matx operator-() const
        {
            Matx result(rows_, cols_);
            for (int c = 0; c < cols_; c++)
                for (int r = 0; r < rows_; r++)
                    result(r, c) = -get_(r, c);
            return result;
        }

        Matx sqrt()
        {
            Matx result(rows_, cols_);
            for (int c = 0; c < cols_; c++)
                for (int r = 0; r < rows_; r++)
                    result(r, c) = std::sqrt(get_(r, c));
            return result;
        }

        // Frobenius norm
        Type norm() const
        {
            Type sum = Type(0);
            for (int c = 0; c < cols_; c++)
                for (int r = 0; r < rows_; r++)
                    sum += get_(r, c) * get_(r, c);
            return std::sqrt(sum);
        }

        // Element accessors (row, col)
        Type &operator()(int r, int c)
        {
            return get_(r, c);
        }

        Type operator()(int r, int c) const
        {
            return get_(r, c);
        }

        // For vector-like usage (assumes single column, i.e. col = 0)
        Type &operator()(int r)
        {
            assert(cols_ == 1);

            return get_(r, 0);
        }

        Type operator()(int r) const
        {
            assert(cols_ == 1);

            return get_(r, 0);
        }

        Type *data()
        {
            return data_;
        }

        const Type *data() const
        {
            return data_;
        }

        // Dimension accessors
        int rows() const { return rows_; }
        int cols() const { return cols_; }
        int size() const { return rows_ * cols_; }

    protected:
        Type &get_(int r, int c)
        {
            //  column major
            //   return data_[r * _cols + c];
            //  row major
            return data_[c * rows_ + r];
        }

        Type get_(int r, int c) const
        {
            //  column major
            //   return data_[r * _cols + c];
            //  row major
            return data_[c * rows_ + r];
        }

        std::unique_ptr<Type[]> data_;
        int rows_, cols_;
    };

    template <typename Type>
    Matx<Type> operator*(const Matx<Type> &m, Type s)
    {
        Matx<Type> result(m.rows(), m.cols());
        for (int c = 0; c < m.cols(); c++)
            for (int r = 0; r < m.rows(); r++)
                result(r, c) = m(r, c) * s;
        return result;
    }

    template <typename Type>
    Matx<Type> operator*(Type s, const Matx<Type> &m)
    {
        return m * s;
    }

    template <typename Type>
    Matx<Type> operator/(const Matx<Type> &m, Type s)
    {
        Matx<Type> result(m.rows(), m.cols());
        for (int c = 0; c < m.cols(); c++)
            for (int r = 0; r < m.rows(); r++)
                result(r, c) = m(r, c) / s;
        return result;
    }

    template <typename Type>
    class VecxR : public Matx<Type>
    {
    public:
        VecxR(int cols) : Matx<Type>(1, cols) {}
        // VecR(const Matx<Type> &mat)
        //     : Matx<Type>(mat) // call the base-class copy constructor
        //{
        // }

        static VecxR Zero(int cols)
        {
            VecxR result(cols);
            for (int i = 0; i < cols; i++)
                result(i) = Type(0);
            return result;
        }

        // Element accessors (row, col)
        Type &operator()(int c)
        {
            return this->get_(0, c);
        }

        Type operator()(int c) const
        {
            return this->get_(0, c);
        }
    };

    template <typename Type>
    class VecxC : public Matx<Type>
    {
    public:
        VecxC(int rows) : Matx<Type>(rows, 1) {}
        // VecxC(const Matx<Type> &mat)
        //     : Matx<Type>(mat) // call the base-class copy constructor
        // {
        //}

        static VecxC Zero(int rows)
        {
            VecxC result(rows);
            for (int i = 0; i < rows; i++)
                result(i) = Type(0);
            return result;
        }

        // Element accessors (row, col)
        Type &operator()(int r)
        {
            return this->get_(r, 0);
        }

        Type operator()(int r) const
        {
            return this->get_(r, 0);
        }
    };
}