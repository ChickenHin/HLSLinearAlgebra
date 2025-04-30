#pragma once

#include "hls_math.h"

namespace linalgHLS
{
    //============================================================
    // Basic fixed-size matrix class
    //============================================================

    template <typename Type, int _rows, int _cols>
    class Mat
    {
    public:
        // Default constructor: initialize to zero
        Mat()
        {
            //*this = Zero();
        }

        Mat(const Type _data[_rows * _cols])
        {
        mat_const_data_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_init_loop_c:
                for (int c = 0; c < _cols; c++)
                    data[r][c] = _data[r * _cols + c];
        }

        // Copy constructor
        Mat(const Mat &other)
        {
        mat_const_other_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_const_other_loop_c:
                for (int c = 0; c < _cols; c++)
                    data[r][c] = other.data[r][c];
        }

        // Assignment operator
        Mat &operator=(const Mat &other)
        {
            // if (this != &other)
            {
            mat_assign_loop_r:
                for (int r = 0; r < _rows; r++)
                mat_assign_loop_c:
                    for (int c = 0; c < _cols; c++)
                        data[r][c] = other.data[r][c];
            }
            return *this;
        }

        // Static "Zero" constructor
        static Mat Zero()
        {
            Mat result;
        mat_zero_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_zero_loop_c:
                for (int c = 0; c < _cols; c++)
                    result.data[r][c] = Type(0);
            return result;
        }

        // Static "Identity" constructor (square matrices only!)
        static Mat Identity()
        {
            // static_assert(_rows == _cols, "Identity only makes sense for square matrices");
            Mat result = Zero();
        mat_identity_loop_i:
            for (int i = 0; i < _rows; i++)
                result.data[i][i] = Type(1);
            return result;
        }

        // Dimension accessors
        static constexpr int rows() { return _rows; }
        static constexpr int cols() { return _cols; }
        static constexpr int size() { return _rows * _cols; }

        // Transpose
        Mat<Type, _cols, _rows> transpose() const
        {
            Mat<Type, _cols, _rows> result;
        mat_tran_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_tran_loop_c:
                for (int c = 0; c < _cols; c++)
                    result(c, r) = data[r][c];
            return result;
        }

        // Matrix multiplication
        template <typename Type2, int __rows, int __cols>
        Mat<Type, _rows, __cols> operator*(const Mat<Type2, __rows, __cols> &rhs) const
        {
            // static_assert(_cols == __rows, "Inner dimensions must match for matrix multiplication");

            Mat<Type, _rows, __cols> result = Mat<Type, _rows, __cols>::Zero();
        mat_mult_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_mult_loop_c:
                for (int c = 0; c < __cols; c++)
                mat_mult_loop_k:
                    for (int k = 0; k < _cols; k++)

                        result(r, c) += data[r][k] * rhs(k, c);

            return result;
        }

        // Scalar multiplication
        // template <typename S> //, typename = std::enable_if_t<std::is_arithmetic_v<S>>>
        Mat operator*(float scalar) const
        {
            Mat result;
        mat_fmult_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_fmult_loop_c:
                for (int c = 0; c < _cols; c++)
                    result.data[r][c] = Type(data[r][c] * scalar);
            return result;
        }

        // Scalar division
        // template <typename S> //, typename = std::enable_if_t<std::is_arithmetic_v<S>>>
        Mat operator/(float scalar) const
        {
            Mat result;
        mat_fdiv_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_fdev_loop_c:
                for (int c = 0; c < _cols; c++)
                    result.data[r][c] = Type(data[r][c] / scalar);
            return result;
        }

        template <typename OutType, typename InType>
        OutType conv(const Mat<InType, _rows, _cols> &rhs) const
        {
            OutType result = Type(0);
        mat_conv_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_conv_loop_k:
                for (int k = 0; k < _cols; k++)
                    result += OutType(data[r][k] * rhs(r, k));

            return result;
        }

        Mat operator+(const Mat &other) const
        {
            Mat result;
        mat_add_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_add_loop_c:
                for (int c = 0; c < _cols; c++)
                    result.data[r][c] = data[r][c] + other.data[r][c];
            return result;
        }

        Mat operator-(const Mat &other) const
        {
            Mat result;
        mat_sub_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_sub_loop_c:
                for (int c = 0; c < _cols; c++)
                    result.data[r][c] = data[r][c] - other.data[r][c];
            return result;
        }

        // Frobenius norm
        Type norm() const
        {
            Type sum = Type(0);
        mat_norm_loop_r:
            for (int r = 0; r < _rows; r++)
            mat_norm_loop_c:
                for (int c = 0; c < _cols; c++)
                    sum += data[r][c] * data[r][c];
            return hls::sqrt(sum);
        }

        // Element accessors (row, col)
        Type &operator()(int r, int c)
        {
            return data[r][c];
        }

        Type operator()(int r, int c) const
        {
            return data[r][c];
        }

        // For vector-like usage (assumes single column, i.e. col = 0)
        Type &operator()(int r)
        {
            static_assert(_cols == 1, "Single-index operator() only valid for Nx1 matrices (vectors)");
            return data[r][0];
        }

        Type operator()(int r) const
        {
            static_assert(_cols == 1, "Single-index operator() only valid for Nx1 matrices (vectors)");
            return data[r][0];
        }

    protected:
        Type data[_rows][_cols];
    };

    template <typename Scalar,          // e.g. int, float, double …
              typename T, int R, int C> //,
                                        // typename = std::enable_if_t<std::is_arithmetic_v<Scalar>>>
    Mat<T, R, C> operator*(Scalar s, const Mat<T, R, C> &m)
    {
        return m * s;
    }

    //============================================================
    // Various Vector specializations (just Nx1 Mat)
    //============================================================

    template <typename Type>
    class Vec1 : public Mat<Type, 1, 1>
    {
    public:
        Vec1() : Mat<Type, 1, 1>() {}
        Vec1(Type x)
        {
            Mat<Type, 1, 1>::data[0] = x;
        }
    };

    template <typename Type>
    class Vec2 : public Mat<Type, 2, 1>
    {
    public:
        Vec2() : Mat<Type, 2, 1>() {}
        Vec2(const Mat<Type, 2, 1> &mat)
            : Mat<Type, 2, 1>(mat) // call the base-class copy constructor
        {
        }
        Vec2(Type x, Type y)
        {
            (*this)(0) = x;
            (*this)(1) = y;
        }
    };

    template <typename Type>
    class Vec3 : public Mat<Type, 3, 1>
    {
    public:
        Vec3() : Mat<Type, 3, 1>() {}
        Vec3(const Mat<Type, 3, 1> &mat)
            : Mat<Type, 3, 1>(mat) // call the base-class copy constructor
        {
        }

        Vec3(Type x, Type y, Type z)
        {
            (*this)(0) = x;
            (*this)(1) = y;
            (*this)(2) = z;
        }

        // Cross product
        Vec3<Type> cross(const Vec3<Type> &other) const
        {
            Vec3<Type> result;
            result(0) = (*this)(1) * other(2) - (*this)(2) * other(1);
            result(1) = (*this)(2) * other(0) - (*this)(0) * other(2);
            result(2) = (*this)(0) * other(1) - (*this)(1) * other(0);
            return result;
        }
    };

    template <typename Type>
    class Vec4 : public Mat<Type, 4, 1>
    {
    public:
        Vec4() : Mat<Type, 4, 1>() {}
        Vec4(const Mat<Type, 4, 1> &mat)
            : Mat<Type, 4, 1>(mat) // call the base-class copy constructor
        {
        }

        Vec4(Type x, Type y, Type z, Type w)
        {
            (*this)(0) = x;
            (*this)(1) = y;
            (*this)(2) = z;
            (*this)(3) = w;
        }

        Vec2<Type> xy()
        {
            return Vec2<Type>((*this)(0), (*this)(1));
        }
    };

    template <typename Type>
    class Vec5 : public Mat<Type, 5, 1>
    {
    public:
        Vec5() : Mat<Type, 5, 1>() {}
        Vec5(Type x, Type y, Type z, Type w, Type a)
        {
            (*this)(0) = x;
            (*this)(1) = y;
            (*this)(2) = z;
            (*this)(3) = w;
            (*this)(4) = a;
        }
    };

    template <typename Type>
    class Vec6 : public Mat<Type, 6, 1>
    {
    public:
        Vec6() : Mat<Type, 6, 1>() {}
        Vec6(Type x, Type y, Type z, Type a, Type b, Type c)
        {
            (*this)(0) = x;
            (*this)(1) = y;
            (*this)(2) = z;
            (*this)(3) = a;
            (*this)(4) = b;
            (*this)(5) = c;
        }
    };

    template <typename Type>
    class Vec8 : public Mat<Type, 8, 1>
    {
    public:
        Vec8() : Mat<Type, 8, 1>() {}
        Vec8(Type x, Type y, Type z, Type a, Type b, Type c, Type d, Type e)
        {
            (*this)(0) = x;
            (*this)(1) = y;
            (*this)(2) = z;
            (*this)(3) = a;
            (*this)(4) = b;
            (*this)(5) = c;
            (*this)(6) = d;
            (*this)(7) = e;
        }
    };

    //============================================================
    // Common small matrix specializations
    //============================================================

    template <typename Type>
    class Mat2 : public Mat<Type, 2, 2>
    {
    public:
        Mat2() : Mat<Type, 2, 2>() {}
        Mat2(const Mat<Type, 2, 2> &mat)
            : Mat<Type, 2, 2>(mat) // call the base-class copy constructor
        {
        }
    };

    template <typename Type>
    class Mat3 : public Mat<Type, 3, 3>
    {
    public:
        Mat3() : Mat<Type, 3, 3>() {}

        Mat3(const Mat<Type, 3, 3> &mat)
            : Mat<Type, 3, 3>(mat) // call the base-class copy constructor
        {
        }

        Mat3(const Type _data[3 * 3])
        {
            const Mat3<Type> &m = *this;
        mat3_const_loop_r:
            for (int r = 0; r < 3; r++)
            mat3_const_loop_c:
                for (int c = 0; c < 3; c++)
                    m(r, c) = _data[r * 3 + c];
        }

        // 3×3 determinant
        Type determinant() const
        {
            const auto &m = *this;
            return m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) - m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) + m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
        }

        // Inverse of 3×3
        Mat3<Type> inverse() const
        {
            Mat3<Type> inv;
            const Mat3<Type> &m = *this;

            Type det = determinant();
            /*
            if (std::fabs(det) < Type(1e-12))
            {
                throw std::runtime_error("Encountered near-zero determinant in Mat3::inverse()");
            }
            */

            Type invDet = Type(1) / det;

            inv(0, 0) = (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) * invDet;
            inv(0, 1) = -(m(0, 1) * m(2, 2) - m(0, 2) * m(2, 1)) * invDet;
            inv(0, 2) = (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * invDet;

            inv(1, 0) = -(m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) * invDet;
            inv(1, 1) = (m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0)) * invDet;
            inv(1, 2) = -(m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) * invDet;

            inv(2, 0) = (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)) * invDet;
            inv(2, 1) = -(m(0, 0) * m(2, 1) - m(0, 1) * m(2, 0)) * invDet;
            inv(2, 2) = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) * invDet;

            return inv;
        }
    };

    template <typename Type>
    class Mat4 : public Mat<Type, 4, 4>
    {
    public:
        Mat4() : Mat<Type, 4, 4>() {}
        Mat4(const Mat<Type, 4, 4> &mat)
            : Mat<Type, 4, 4>(mat) // call the base-class copy constructor
        {
        }
    };

    template <typename Type>
    class Mat6 : public Mat<Type, 6, 6>
    {
    public:
        Mat6() : Mat<Type, 6, 6>() {}
    };

    template <typename Type>
    class Mat8 : public Mat<Type, 8, 8>
    {
    public:
        Mat8() : Mat<Type, 8, 8>() {}
    };

    //============================================================
    // Basic Quaternion
    //============================================================

    template <typename Type>
    class Quaternion
    {
    public:
        Quaternion() : w_(Type(1)), x_(Type(0)), y_(Type(0)), z_(Type(0)) {}

        Quaternion(Type w, Type x, Type y, Type z)
            : w_(w), x_(x), y_(y), z_(z)
        {
        }

        Mat3<Type> matrix() const
        {
            Mat3<Type> matrix_;

            // Using the standard formula
            Type xx = Type(2) * x_ * x_;
            Type yy = Type(2) * y_ * y_;
            Type zz = Type(2) * z_ * z_;
            Type xy = Type(2) * x_ * y_;
            Type xz = Type(2) * x_ * z_;
            Type yz = Type(2) * y_ * z_;
            Type wx = Type(2) * w_ * x_;
            Type wy = Type(2) * w_ * y_;
            Type wz = Type(2) * w_ * z_;

            matrix_(0, 0) = Type(1) - (yy + zz);
            matrix_(0, 1) = xy - wz;
            matrix_(0, 2) = xz + wy;

            matrix_(1, 0) = xy + wz;
            matrix_(1, 1) = Type(1) - (xx + zz);
            matrix_(1, 2) = yz - wx;

            matrix_(2, 0) = xz - wy;
            matrix_(2, 1) = yz + wx;
            matrix_(2, 2) = Type(1) - (xx + yy);

            return matrix_;
        }

        // Multiply quaternion by another quaternion
        Quaternion operator*(const Quaternion &q) const
        {
            // (w1, x1, y1, z1) * (w2, x2, y2, z2)
            // = (w1*w2 - x1*x2 - y1*y2 - z1*z2,
            //    w1*x2 + x1*w2 + y1*z2 - z1*y2,
            //    w1*y2 - x1*z2 + y1*w2 + z1*x2,
            //    w1*z2 + x1*y2 - y1*x2 + z1*w2)
            return Quaternion(
                w_ * q.w_ - x_ * q.x_ - y_ * q.y_ - z_ * q.z_,
                w_ * q.x_ + x_ * q.w_ + y_ * q.z_ - z_ * q.y_,
                w_ * q.y_ - x_ * q.z_ + y_ * q.w_ + z_ * q.x_,
                w_ * q.z_ + x_ * q.y_ - y_ * q.x_ + z_ * q.w_);
        }

        // Rotate a 3D vector by this quaternion (assumed normalized)
        Vec3<Type> operator*(const Vec3<Type> &v) const
        {
            // Convert v to a pure quaternion with zero real part
            Quaternion vq(Type(0), v(0), v(1), v(2));

            // q * vq
            Quaternion qv = (*this) * vq;
            // conj(q) = (w, -x, -y, -z)
            Quaternion qc(w_, -x_, -y_, -z_);

            // rotated = qv * conj(q)
            Quaternion rotated = qv * qc;

            // The imaginary part is the rotated vector
            return Vec3<Type>(rotated.x_, rotated.y_, rotated.z_);
        }

        // Inverse (for normalized quaternions, inverse = conjugate)
        Quaternion inverse() const
        {
            Type normSq = w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_;
            // if (std::fabs(normSq) < Type(1e-12))
            //     throw std::runtime_error("Near-zero norm in Quaternion::inverse()");

            Type inv = Type(1) / normSq;
            return Quaternion(w_ * inv, -x_ * inv, -y_ * inv, -z_ * inv);
        }

        // Public components for convenience
        Type w_, x_, y_, z_;
    };

    //============================================================
    // SO3 class (3D rotation), stored as a 3×3 matrix
    //============================================================

    template <typename Type>
    class SO3
    {
    public:
        SO3()
        {
            // Set to identity
            // matrix_ = Mat3<Type>::Identity();
            quaternion_ = Quaternion<Type>(Type(1), Type(0), Type(0), Type(0));
        }
        /*
        // Construct directly from a 3×3
        SO3(const Mat3<Type> &m)
        {
            matrix_ = m;
        }
        */

        // Build from a quaternion
        SO3(Type qw, Type qx, Type qy, Type qz)
        {
            // fromQuaternion(qw, qx, qy, qz);
            quaternion_ = Quaternion<Type>(qw, qx, qy, qz);
        }

        SO3(Quaternion<Type> q)
        {
            // fromQuaternion(q.w_, q.x_, q.y_, q.z_);
            quaternion_ = q;
        }
        SO3(const SO3<Type> &other)
        {
            // matrix_ = other.matrix_;
            quaternion_ = other.quaternion_;
        }

        void setQuaternion(const Quaternion<Type> &q)
        {
            // fromQuaternion(q.w_, q.x_, q.y_, q.z_);
            quaternion_ = q;
        }

        Mat3<Type> matrix() const
        {
            return quaternion_.matrix();
        }

        /*
        // Convert quaternion -> 3×3 rotation (assuming unit quaternion)
        void fromQuaternion(Type qw, Type qx, Type qy, Type qz)
        {
            // Using the standard formula
            Type xx = Type(2) * qx * qx;
            Type yy = Type(2) * qy * qy;
            Type zz = Type(2) * qz * qz;
            Type xy = Type(2) * qx * qy;
            Type xz = Type(2) * qx * qz;
            Type yz = Type(2) * qy * qz;
            Type wx = Type(2) * qw * qx;
            Type wy = Type(2) * qw * qy;
            Type wz = Type(2) * qw * qz;

            matrix_(0, 0) = Type(1) - (yy + zz);
            matrix_(0, 1) = xy - wz;
            matrix_(0, 2) = xz + wy;

            matrix_(1, 0) = xy + wz;
            matrix_(1, 1) = Type(1) - (xx + zz);
            matrix_(1, 2) = yz - wx;

            matrix_(2, 0) = xz - wy;
            matrix_(2, 1) = yz + wx;
            matrix_(2, 2) = Type(1) - (xx + yy);
        }
        */

        // Identity
        void setIdentity()
        {
            // matrix_ = Mat3<Type>::Identity();
            quaternion_ = Quaternion<Type>(Type(1), Type(0), Type(0), Type(0));
        }

        // Inverse = transpose for rotation matrix
        SO3<Type> inverse() const
        {
            // return SO3<Type>(matrix_.transpose());
            return SO3<Type>(quaternion_.inverse());
        }

        // Element access
        // Type operator()(int r, int c) const { return matrix_(r, c); }
        // Type &operator()(int r, int c) { return matrix_(r, c); }

        // Rotation of a vector
        Vec3<Type> operator*(const Vec3<Type> &v) const
        {
            // return matrix_ * v;
            return quaternion_ * v;
        }

        // Composition of two SO3 rotations
        SO3<Type> operator*(const SO3<Type> &rhs) const
        {
            // return SO3<Type>(matrix_ * rhs.matrix_);
            return SO3<Type>(quaternion_ * rhs.quaternion_);
        }

        // Get the underlying 3×3
        // const Mat3<Type> &matrix() const { return matrix_; }
        // Mat3<Type> &matrix() { return matrix_; }

    private:
        // Mat3<Type> matrix_;
        Quaternion<Type> quaternion_;
    };

    //============================================================
    // Exponential map for so(3) -> SO3
    //    exp: R^3 (axis*angle) -> 3×3 rotation
    //============================================================
    // A helper for the skew-symmetric matrix ("wedge")
    template <typename Type>
    Mat3<Type> wedge(const Vec3<Type> &phi)
    {
        Mat3<Type> w = Mat3<Type>::Zero();
        w(0, 0) = Type(0);
        w(0, 1) = -phi(2);
        w(0, 2) = phi(1);
        w(1, 0) = phi(2);
        w(1, 1) = Type(0);
        w(1, 2) = -phi(0);
        w(2, 0) = -phi(1);
        w(2, 1) = phi(0);
        w(2, 2) = Type(0);
        return w;
    }

    // Outer product of two 3D vectors -> 3×3 matrix
    template <typename Type>
    Mat3<Type> outerProduct(const Vec3<Type> &a, const Vec3<Type> &b)
    {
        Mat3<Type> m = Mat3<Type>::Zero();
    mat3_out_loop_r:
        for (int r = 0; r < 3; r++)
        mat_out_loop_c:
            for (int c = 0; c < 3; c++)
                m(r, c) = a(r) * b(c);
        return m;
    }

    // Exponential map for so(3)
    template <typename Type>
    SO3<Type> so3Exp(const Vec3<Type> &phi)
    {
        Type angle = phi.norm();
        if (angle < Type(1e-12))
        {
            // Near zero, use approximation: exp(phi) ~ I + wedge(phi)
            Mat3<Type> approx = Mat3<Type>::Identity() + wedge(phi);
            return SO3<Type>(approx);
        }

        Vec3<Type> axis = (phi / angle);
        Type s = hls::sin(angle);
        Type c = hls::cos(angle);

        // Rodrigues' formula: R = I c + (1-c) (axis axis^T) + [axis]_x s
        Mat3<Type> R = Mat3<Type>::Identity() * c + outerProduct(axis, axis) * (Type(1) - c) + wedge(axis) * s;

        return SO3<Type>(R);
    }

    //============================================================
    // Left Jacobian of SO3
    //   This is a 3×3 matrix used in many Lie-theory-based derivations
    //============================================================
    template <typename Type>
    Mat3<Type> so3LeftJacobian(const Vec3<Type> &phi)
    {
        Type angle = phi.norm();
        if (angle < Type(1e-12))
        {
            // Approx: J_l(phi) ~ I + 0.5 [phi]_x
            Mat3<Type> result = Mat3<Type>::Identity() + wedge(phi) * Type(0.5);
            return result;
        }

        Vec3<Type> axis = (phi / angle);
        Type s = hls::sin(angle);
        Type c = hls::cos(angle);

        Mat3<Type> I = Mat3<Type>::Identity();
        Mat3<Type> K = wedge(axis);
        Mat3<Type> aaT = outerProduct(axis, axis);

        // Formula: J_l(phi) = I + ( (1 - c)/angle^2 ) [phi]_x + (angle - s)/angle^3 [phi]_x^2
        // But one commonly used expression is:
        // (s/angle)*I + (1 - s/angle)*aaT + ((1 - c)/angle)*K
        // Both are valid forms. Use whichever is standard for you:

        Mat3<Type> term1 = I * (s / angle);
        Mat3<Type> term2 = aaT * (Type(1) - s / angle);
        Mat3<Type> term3 = K * ((Type(1) - c) / angle);

        return (term1 + term2 + term3);
    }

    //============================================================
    // SE3 class = (SO3 rotation, R^3 translation)
    //============================================================
    template <typename Type>
    class SE3
    {
    public:
        SE3()
        {
            rot_ = SO3<Type>();
            trans_ = Vec3<Type>(Type(0), Type(0), Type(0));
        }

        SE3(const SO3<Type> &r, const Vec3<Type> &t)
        {
            rot_ = r;
            trans_ = t;
        }

        void setQuaternion(const Quaternion<Type> &q)
        {
            rot_.setQuaternion(q);
        }

        Mat4<Type> matrix() const
        {
            Mat4<Type> mat = Mat4<Type>::Zero();
            Mat3<Type> R = rot_.matrix();
        se3_matrix_loop_r:
            for (int r = 0; r < 3; r++)
            se3_matrix_loop_c:
                for (int c = 0; c < 3; c++)
                    mat(r, c) = R(r, c);
            mat(3, 0) = trans_(0);
            mat(3, 1) = trans_(1);
            mat(3, 2) = trans_(2);
            mat(3, 3) = Type(1);
            return mat;
        }

        // Transform a point
        Vec3<Type> operator*(const Vec3<Type> &p) const
        {
            return rot_ * p + trans_;
        }

        // Composition of two SE3
        SE3<Type> operator*(const SE3<Type> &rhs) const
        {
            // [R1|t1] [R2|t2] = [R1R2 | R1 t2 + t1]
            SE3<Type> out;
            out.rot_ = rot_ * rhs.rot_;
            out.trans_ = rot_ * rhs.trans_ + trans_;
            return out;
        }

        // Inverse
        SE3<Type> inverse() const
        {
            // Inv( [R|t] ) = [R^T | -R^T t]
            SE3<Type> inv;
            inv.rot_ = rot_.inverse();
            inv.trans_ = inv.rot_ * (trans_ * Type(-1));
            return inv;
        }

        // Get rotation, translation
        SO3<Type> &rotation() { return rot_; }
        const SO3<Type> &rotation() const { return rot_; }
        Vec3<Type> &translation() { return trans_; }
        const Vec3<Type> &translation() const { return trans_; }

    private:
        SO3<Type> rot_;
        Vec3<Type> trans_;
    };

    //============================================================
    // Exponential map SE3: R^6 -> SE3
    //   xi = (rho, phi) in R^3 x R^3
    //============================================================
    template <typename Type>
    SE3<Type> se3Exp(const Vec6<Type> &xi)
    {
        // xi = (rho, phi), each 3D
        Vec3<Type> rho(xi(0), xi(1), xi(2));
        Vec3<Type> phi(xi(3), xi(4), xi(5));

        // Rotation part
        SO3<Type> R = so3Exp(phi);

        // Translation part: J_l(phi) * rho
        Mat3<Type> J = so3LeftJacobian(phi);
        Vec3<Type> t = J * rho;

        return SE3<Type>(R, t);
    }

} // namespace linalg