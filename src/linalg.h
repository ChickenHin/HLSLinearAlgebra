#pragma once

#include <cmath>
#include <cassert>

namespace linalg
{
    template <typename Type, int _rows, int _cols>
    class Mat
    {
        Mat()
        {
            this == Zero();
        }

        Mat(const Mat &other)
        {
            for (int y = 0; y < _rows; y++)
                for (int x = 0; x < _cols; x++)
                {
                    data[y][x] = other(y, x);
                }
        }

        Mat &operator=(const Mat &other)
        {
            if (this != &other)
            {
                for (int y = 0; y < _rows; y++)
                    for (int x = 0; x < _cols; x++)
                    {
                        data[y][x] = other(y, x);
                    }
            }
            return *this;
        }

        static constexpr Mat<Type, _rows, _cols> Zero()
        {
            Mat<Type, _rows, _cols> result;

            for (int y = 0; y < _rows; y++)
                for (int x = 0; x < _cols; x++)
                {
                    result.data[y][x] = 0.0;
                }
            return result;
        }

        static constexpr Mat<Type, _rows, _cols> Identity()
        {
            Mat<Type, _rows, _cols> result;

            for (int y = 0; y < _rows; y++)
                for (int x = 0; x < _cols; x++)
                {
                    if (x == y)
                        result.data[y][x] = 1.0;
                    else
                        result.data[y][x] = 0.0;
                }
            return result;
        }

        static constexpr int rows()
        {
            return _rows;
        }

        static constexpr int cols()
        {
            return _cols;
        }

        static constexpr int size()
        {
            return _rows * _cols;
        }

        Mat<Type, _cols, _rows> transpose() const
        {
            Mat<Type, _cols, _rows> result;

            for (int y = 0; y < _rows; y++)
            {
                for (int x = 0; x < _cols; x++)
                {
                    result.data[x][y] = data[y][x];
                }
            }

            return result;
        }

        template <typename Type2>
        Mat operator/(Type2 c) const
        {
            Mat result;

            for (int y = 0; y < _rows; y++)
                for (int x = 0; x < _cols; x++)
                {
                    result.data[y][x] = data[y][x] / c;
                }

            return result;
        }

        template <typename Type2>
        Mat operator*(Type2 c) const
        {
            Mat result;

            for (int y = 0; y < _rows; y++)
                for (int x = 0; x < _cols; x++)
                {
                    result.data[y][x] = data[y][x] * c;
                }

            return result;
        }

        template <typename Type2, int __rows, int __cols>
        Mat<Type, _rows, _cols> operator*(Mat<Type2, __rows, __cols> c)
        {
            assert(_cols == __rows);
            assert(_rows == __cols);

            Mat<Type, _rows, __cols> result;

            for (int y = 0; y < _rows; y++)
                for (int x = 0; x < __cols; x++)
                {
                    for (int z = 0; z < _cols; z++)
                    {
                        result.data[y][x] += data[y][z] * c(z, x);
                    }
                }

            return result;
        }

        Type &operator()(int b, int c)
        {
            return data[b][c];
        }

        Type operator()(int b, int c) const
        {
            return data[b][c];
        }

        Type &operator()(int b)
        {
            return data[b][0];
        }

        Type operator()(int b) const
        {
            return data[b][0];
        }

    protected:
        Type data[_rows][_cols];
    };
    /*
    template <typename type>
    class Matx
    {
        Matx()
        {
            data = nullptr;
            _rows = 0;
            _cols = 0;
        }

        Matx(int __rows, int __cols)
        {
            _rows = __rows;
            _cols = __cols;
            data = new type[_rows * _cols];

            setZero();
        }

        Matx(const Matx &other)
        {
            _rows = other.rows();
            _cols = other.cols();

            data = new type[_rows * _cols];

            for (int y = 0; y < _rows; y++)
                for (int x = 0; x < _cols; x++)
                    data[y + _rows * x] = other(y, x);
        }

        void setZero()
        {
            for (int y = 0; y < _rows; y++)
                for (int x = 0; x < _cols; x++)
                {
                    data[y + _rows * x] = 0.0;
                }
        }

        static constexpr matx Zero(int __rows, int __cols)
        {
            matx v(__rows, __cols);
            for (int y = 0; y < __rows; y++)
                for (int x = 0; x < __cols; x++)
                    v(y, x) = 0;
            return v;
        }

        int rows() const
        {
            return _rows;
        }

        int cols() const
        {
            return _cols;
        }

        int size()
        {
            return _rows * _cols;
        }

        Matx transpose()
        {
            matx result(_cols, _rows);

            for (int y = 0; y < _rows; y++)
            {
                for (int x = 0; x < _cols; x++)
                {
                    result(x, y) = data[y + _rows * x];
                }
            }

            return result;
        }

        Matx operator*(Matx c)
        {
            assert(_cols == c.rows());
            assert(_rows == c.cols());

            Matx result(_rows, c.cols());

            for (int y = 0; y < _rows; y++)
                for (int x = 0; x < c.cols(); x++)
                {
                    for (int z = 0; z < _cols; z++)
                    {
                        result(y, x) += data[y + _rows * z] * c(z, x);
                    }
                }

            return result;
        }

        vecx<type> operator*(vecx<type> c)
        {
            vecx<type> result(_rows);

            for (int y = 0; y < _rows; y++)
                for (int z = 0; z < _cols; z++)
                    result(y) += data[y + _rows * z] * c(z);

            return result;
        }

        matx &operator=(const matx &other)
        {
            if (this != &other)
            {
                if (data != nullptr)
                    delete[] data;

                _rows = other.rows();
                _cols = other.cols();

                data = new type[_rows * _cols];

                for (int y = 0; y < _rows; y++)
                    for (int x = 0; x < _cols; x++)
                        data[y + _rows * x] = other(y, x);
            }
            return *this;
        }

        matx operator*(type c)
        {
            matx result = matx(_rows, _cols);

            for (int y = 0; y < _rows; y++)
                for (int x = 0; x < _cols; x++)
                    result(y, x) = data[y + _rows * x] * c;

            return result;
        }

        type &operator()(int c, int d)
        {
            return data[c + _rows * d];
        }

        type operator()(int c, int d) const
        {
            return data[c + _rows * d];
        }

    protected:
        type *data;
        int _rows;
        int _cols;
    };
    */
    template <typename Type>
    class Vec1 : public Mat<Type, 1, 1>
    {
        Vec1() : Mat<Type, 1, 1>()
        {
        }

        Vec1(Type x)
        {
            this(0, 0) = x;
        }
    };

    template <typename Type>
    class Vec2 : public Mat<Type, 2, 1>
    {
        Vec2() : Mat<Type, 2, 1>()
        {
        }

        Vec2(Type x, Type y)
        {
            this(0, 0) = x;
            this(1, 0) = y;
        }
    };

    template <typename Type>
    class Vec3 : public Mat<Type, 3, 1>
    {
        Vec3() : Mat<Type, 3, 1>()
        {
        }

        Vec3(Type x, Type y, Type z)
        {
            *this(0, 0) = x;
            *this(1, 0) = y;
            *this(2, 0) = z;
        }

        Vec3<Type> cross(Vec3<Type> a)
        {
            Vec3<Type> result;
            result(0) = Mat<Type, 1, 3>::data[1] * a(2) - Mat<Type, 1, 3>::data[2] * a(1);
            result(1) = Mat<Type, 1, 3>::data[2] * a(0) - Mat<Type, 1, 3>::data[0] * a(2);
            result(2) = Mat<Type, 1, 3>::data[0] * a(1) - Mat<Type, 1, 3>::data[1] * a(0);
            return result;
        }
    };

    template <typename Type>
    class Vec4 : public Mat<Type, 4, 1>
    {
        Vec4() : Mat<Type, 4, 1>()
        {
        }

        Vec4(Type x, Type y, Type z, Type w)
        {
            *this(0, 0) = x;
            *this(1, 0) = y;
            *this(2, 0) = z;
            *this(3, 0) = w;
        }
    };

    template <typename Type>
    class Vec5 : public Mat<Type, 5, 1>
    {
        Vec5() : Mat<Type, 5, 1>()
        {
        }

        Vec5(Type x, Type y, Type z, Type w, Type a)
        {
            *this(0, 0) = x;
            *this(1, 0) = y;
            *this(2, 0) = z;
            *this(3, 0) = w;
            *this(4, 0) = a;
        }
    };

    template <typename Type>
    class Vec6 : public Mat<Type, 6, 1>
    {
        Vec6() : Mat<Type, 6, 1>()
        {
        }

        Vec6(Type x, Type y, Type z, Type a, Type b, Type c)
        {
            *this(0, 0) = x;
            *this(1, 0) = y;
            *this(2, 0) = z;
            *this(3, 0) = a;
            *this(4, 0) = b;
            *this(5, 0) = c;
        }
    };

    template <typename Type>
    class Vec8 : public Mat<Type, 8, 1>
    {
        Vec8() : Mat<Type, 8, 1>()
        {
        }

        Vec8(Type x, Type y, Type z, Type a, Type b, Type c, Type d, Type e)
        {
            *this(0, 0) = x;
            *this(1, 0) = y;
            *this(2, 0) = z;
            *this(3, 0) = a;
            *this(4, 0) = b;
            *this(5, 0) = c;
            *this(6, 0) = d;
            *this(7, 0) = e;
        }
    };

    template <typename Type>
    class Mat3 : public Mat<Type, 3, 3>
    {
        Mat3() : Mat<Type, 3, 3>()
        {
        }

        Type determinant()
        {
            return this(0, 0) * (this(1, 1) * this(2, 2) - this(1, 2) * this(2, 1)) - this(0, 1) * (this(1, 0) * this(2, 2) - mat<type, 3, 3>::data[1][2] * mat<type, 3, 3>::data[2][0]) + mat<type, 3, 3>::data[0][2] * (mat<type, 3, 3>::data[1][0] * mat<type, 3, 3>::data[2][1] - mat<type, 3, 3>::data[1][1] * mat<type, 3, 3>::data[2][0]);
        }

        Mat3<Type> inverse()
        {
            Mat3<Type> inv;

            Type det = determinant();

            if (std::fabs(det) < 1e-12)
            {
                throw std::runtime_error("Encountered near-zero determinant.");
            }

            double invDet = 1.0 / det;

            inv.data[0][0] = (mat<type, 3, 3>::data[1][1] * mat<type, 3, 3>::data[2][2] - mat<type, 3, 3>::data[1][2] * mat<type, 3, 3>::data[2][1]) * invDet;
            inv.data[0][1] = -(mat<type, 3, 3>::data[0][1] * mat<type, 3, 3>::data[2][2] - mat<type, 3, 3>::data[0][2] * mat<type, 3, 3>::data[2][1]) * invDet;
            inv.data[0][2] = (mat<type, 3, 3>::data[0][1] * mat<type, 3, 3>::data[1][2] - mat<type, 3, 3>::data[0][2] * mat<type, 3, 3>::data[1][1]) * invDet;

            inv.data[1][0] = -(mat<type, 3, 3>::data[1][0] * mat<type, 3, 3>::data[2][2] - mat<type, 3, 3>::data[1][2] * mat<type, 3, 3>::data[2][0]) * invDet;
            inv.data[1][1] = (mat<type, 3, 3>::data[0][0] * mat<type, 3, 3>::data[2][2] - mat<type, 3, 3>::data[0][2] * mat<type, 3, 3>::data[2][0]) * invDet;
            inv.data[1][2] = -(mat<type, 3, 3>::data[0][0] * mat<type, 3, 3>::data[1][2] - mat<type, 3, 3>::data[0][2] * mat<type, 3, 3>::data[1][0]) * invDet;

            inv.data[2][0] = (mat<type, 3, 3>::data[1][0] * mat<type, 3, 3>::data[2][1] - mat<type, 3, 3>::data[1][1] * mat<type, 3, 3>::data[2][0]) * invDet;
            inv.data[2][1] = -(mat<type, 3, 3>::data[0][0] * mat<type, 3, 3>::data[2][1] - mat<type, 3, 3>::data[0][1] * mat<type, 3, 3>::data[2][0]) * invDet;
            inv.data[2][2] = (mat<type, 3, 3>::data[0][0] * mat<type, 3, 3>::data[1][1] - mat<type, 3, 3>::data[0][1] * mat<type, 3, 3>::data[1][0]) * invDet;

            return inv;
        }
    };

    template <typename Type>
    class Mat6 : public Mat<Type, 6, 6>
    {
        Mat6() : Mat<Type, 6, 6>()
        {
        }
    };

    template <typename Type>
    class Mat8 : public Mat<Type, 8, 8>
    {
        Mat8() : Mat<Type, 8, 8>()
        {
        }
    };

    template <typename Type>
    class Quaternion
    {
    public:

        Quaternion(Type w, Type x, Type y, Type z)
            : w_(w), x_(x), y_(y), z_(z)
        {
        }

        Quaternion operator*(const Quaterion &q) const
        {
            double w = w_ * q.w_ - x_ * q.x_ - y_ * q.y_ - z_ * q.z_;
            double x = w_ * q.x_ + x_ * q.w_ + y_ * q.z_ - z_ * q.y_;
            double y = w_ * q.y_ - x_ * q.z_ + y_ * q.w_ + z_ * q.x_;
            double z = w_ * q.z_ + x_ * q.y_ - y_ * q.x_ + z_ * q.w_;

            return Quaterion(w, x, y, z);
        }

        Vec3 operator*(const std::array<double, 3> &v) const
        {
            // Convert v to a "pure quaternion" with zero real part
            Quaterion vQuat(0.0, v(0), v(1), v(2));

            // Conjugate of q (since q is assumed normalized, conj(q) = q^-1)
            Quaterion qConjugate(w_, -x_, -y_, -z_);

            // Compute q * vQuat
            Quaterion qv = Quaterion(w_, x_, y_, z_)*vQuat;

            // Compute (q * vQuat) * q^-1 (which is q^*)
            Quaterion rotated = quaternionMultiply(qv*qConjugate);

            // The rotated vector is in the imaginary part (x, y, z) of the result
            return Vec3(rotated[1], rotated[2], rotated[3]);
        }

        Quaterion inverse() const
        {
            // Squared magnitude
            double normSq = w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_;

            double invNormSq = 1.0 / normSq;

            // Conjugate of q: conj(q) = (w, -x, -y, -z)
            double wInv = w * invNormSq;
            double xInv = -x * invNormSq;
            double yInv = -y * invNormSq;
            double zInv = -z * invNormSq;

            return Quaterion(wInv, xInv, yInv, zInv);
        }
        protected:
            Type w_, x_, y_, z_;
    }

    template <typename Type>
    class SO3
    {
        SO3()
        {
            identity();
        }

        SO3(Type qw, Type qx, Type qy, Type qz)
        {
            fromQuaternion(qw, qx, qy, qz);
        }

        SO3(mat3<Type> mat)
        {
            matrix = mat;
        }

        void fromQuaternion(Type qw, Type qx, Type qy, Type qz)
        {
            const type x = 2 * qx;
            const type y = 2 * qy;
            const type z = 2 * qz;
            const type wx = x * qw;
            const type wy = y * qw;
            const type wz = z * qw;
            const type xx = x * qx;
            const type xy = y * qx;
            const type xz = z * qx;
            const type yy = y * qy;
            const type yz = z * qy;
            const type zz = z * qz;

            matrix(0, 0) = 1 - (yy + zz);
            matrix(0, 1) = xy - wz;
            matrix(0, 2) = xz + wy;
            matrix(1, 0) = xy + wz;
            matrix(1, 1) = 1 - (xx + zz);
            matrix(1, 2) = yz - wx;
            matrix(2, 0) = xz - wy;
            matrix(2, 1) = yz + wx;
            matrix(2, 2) = 1 - (xx + yy);
        }

        /*
        SE3(cv::Mat_<float> r, cv::vec3f t)
        {
          data(0,0)=r(0,0); data(0,1)=r(0,1); data(0,2)=r(0,2); data(0,3)=t.x;
          data(1,0)=r(1,0); data(1,1)=r(1,1); data(1,2)=r(1,2); data(1,3)=t.y;
          data(2,0)=r(2,0); data(2,1)=r(2,1); data(2,2)=r(2,2); data(2,3)=t.z;
        }
        */

        void identity()
        {
            matrix.identity();
        }

        SO3<type> inverse() const
        {
            return matrix.transpose();
        }

        void operator=(SO3<type> c)
        {
            matrix = c.matrix;
        }

        type operator()(int r, int c) const
        {
            return matrix(r, c);
        }

        type &operator()(int r, int c)
        {
            return matrix(r, c);
        }

        vec3<type> dot(const vec3<type> &p) const
        {
            vec3<type> result = matrix.dot(p);
            return result;
        }

    private:
        mat3<type> matrix;
    };

    template <typename type>
    SO3<type> exp(vec3<type> phi)
    {
        type angle = phi.norm();

        // # Near phi==0, use first order Taylor expansion
        //   if np.isclose(angle, 0.):
        //       return cls(np.identity(cls.dim) + cls.wedge(phi))

        vec3<type> axis = phi / angle;
        type s = sin(angle);
        type c = cos(angle);
        mat3<type> mat_result = c * mat3<type>::identity() + (1 - c) * axis.outer(axis) + s * wedge(axis);
        return SO3<type>(mat_result);
    }

    template <typename type>
    mat3<type> wedge(vec3<type> phi)
    {
        SO3<type> r;
        r(0, 0) = 0.0;
        r(0, 1) = -phi(2);
        r(0, 2) = phi(1);
        r(1, 0) = phi(2);
        r(1, 1) = 0.0;
        r(1, 2) = -phi(0);
        r(2, 0) = -phi(1);
        r(2, 1) = phi(0);
        r(2, 2) = 0.0;
        return r;
    }

    template <typename type>
    SO3<type> left_jacobian(vec3<type> phi)
    {
        type angle = phi.norm();

        // # Near |phi|==0, use first order Taylor expansion
        //  if np.isclose(angle, 0.):
        //     return np.identity(cls.dof) + 0.5 * cls.wedge(phi)

        vec3<type> axis = phi / angle;
        type s = sin(angle);
        type c = cos(angle);

        mat3<type> mat = (s / angle) * mat3<type>::identity() +
                         (1 - s / angle) * axis.outer(axis) +
                         ((1 - c) / angle) * wedge(axis);

        return SO3<type>(mat);
    }

    template <typename type>
    class SE3
    {
        SE3()
        {
            identity();
        }

        /// Constructor from a normalized quaternion and a translation vector
        SE3(type qw, type qx, type qy, type qz, type tx, type ty, type tz)
        {
            rot.fromQuaternion(qw, qx, qy, qz);
            tra = vec3<type>(tx, ty, tz);
        }

        /// Construct from C arrays
        /// r is rotation matrix row major
        /// t is the translation vector (x y z)
        SE3(type *r, type *t)
        {
            rot(0, 0) = r[0];
            rot(0, 1) = r[1];
            rot(0, 2) = r[2];

            rot(1, 0) = r[3];
            rot(1, 1) = r[4];
            rot(1, 2) = r[5];

            rot(2, 0) = r[6];
            rot(2, 1) = r[7];
            rot(2, 2) = r[8];

            tra(0) = t[0];
            tra(1) = t[1];
            tra(2) = t[2];
        }

        SE3(SO3<type> _rotation, vec3<type> _translation)
        {
            rot = _rotation;
            tra = _translation;
        }

        /*
        SE3(cv::Mat_<float> r, cv::vec3f t)
        {
          data(0,0)=r(0,0); data(0,1)=r(0,1); data(0,2)=r(0,2); data(0,3)=t.x;
          data(1,0)=r(1,0); data(1,1)=r(1,1); data(1,2)=r(1,2); data(1,3)=t.y;
          data(2,0)=r(2,0); data(2,1)=r(2,1); data(2,2)=r(2,2); data(2,3)=t.z;
        }
        */

        void identity()
        {
            rot.identity();
            tra = vec3<float>(0.0, 0.0, 0.0);
        }

        SE3<type> inverse() const
        {
            _SE3<type> result;
            result.rot = rot.inverse();
            result.tra = -result.rot.dot(tra);
            return result;
        }

        /*
            type this->operator()()(int r, int c) const
            {
              return data[r][c];
            }

            type &this->operator()()(int r, int c)
            {
              return data[r][c];
            }
            */

        SE3<type> operator*(const _SE3<type> &rhs)
        {
            _SE3<type> result;
            result.rot = rot.dot(rhs.rot);
            result.tra = rot.dot(rhs.tra) + tra;
            return result;
        }

        vec3<type> operator*(const vec3<type> &rhs)
        {
            vec3<type> result = rot.dot(rhs) + tra;
            return result;
        }
        /*
            void operator=(SE3<type> c)
            {
                rotation = c.rotation;
                translation = c.translation;
            }
            */

        vec3<type> translation()
        {
            return tra;
        }

        SO3<type> so3()
        {
            return rot;
        }

    private:
        SO3<type> rot;
        vec3<type> tra;
    };

    template <typename type>
    SE3<type> exp(vec6<type> xi)
    {
        vec3<type> rho(xi(0), xi(1), xi(2));
        vec3<type> phi(xi(3), xi(4), xi(5));

        mat3<type> rotation = exp(phi);
        vec3<type> translation = left_jacobian(phi).dot(rho);

        return _SE3<type>(rotation, translation);
    }
}