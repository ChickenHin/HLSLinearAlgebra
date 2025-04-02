#pragma once

#include "hls_math.h"

template <typename type>
struct vec2
{
    vec2()
    {
    }

    vec2(type x, type y)
    {
        data[0] = x;
        data[1] = y;
    }

    vec2<type> operator+(vec2<type> c)
    {
        vec2<type> result;
        result(0) = data[0] + c(0);
        result(1) = data[1] + c(1);
        return result;
    }

    vec2<type> operator-(vec2<type> c)
    {
        vec2<type> result;
        result(0) = data[0] - c(0);
        result(1) = data[1] - c(1);
        return result;
    }

    void operator=(vec2<type> c)
    {
        data[0] = c(0);
        data[1] = c(1);
    }

    type &operator()(int c)
    {
        return data[c];
    }

    type operator()(int c) const
    {
        return data[c];
    }

    type data[2];
};

template <typename type>
struct mat3;

template <typename type>
struct vec3
{
    vec3()
    {
    }

    vec3(type x, type y, type z)
    {
        set(x, y, z);
    }

    void set(type x, type y, type z)
    {
        data[0] = x;
        data[1] = y;
        data[2] = z;
    }

    type dot(vec3<type> a)
    {
        type result = data[0] * a(0) + data[1] * a(1) + data[2] * a(2);
        return result;
    }

    mat3<type> outer(vec3<type> a)
    {
        mat3<type> result;
        // for (int y = 0; y < 3; y++)
        //   for (int x = 0; x < 3; x++)
        //     result(y, x) = data[y] * a(x);

        result(0, 0) = data[0] * a(0);
        result(0, 1) = data[0] * a(1);
        result(0, 2) = data[0] * a(2);

        result(1, 0) = data[1] * a(0);
        result(1, 1) = data[1] * a(1);
        result(1, 2) = data[1] * a(2);

        result(2, 0) = data[2] * a(0);
        result(2, 1) = data[2] * a(1);
        result(2, 2) = data[2] * a(2);

        return result;
    }

    type norm()
    {
        return hls::sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
    }

    template <typename type2>
    vec3<type> operator/(type2 c)
    {
        vec3<type> result;
        result(0) = data[0] / c;
        result(1) = data[1] / c;
        result(2) = data[2] / c;
        return result;
    }

    vec3<type> operator+(vec3<type> c)
    {
        vec3<type> result;
        result(0) = data[0] + c(0);
        result(1) = data[1] + c(1);
        result(2) = data[2] + c(2);
        return result;
    }

    void operator=(vec3<type> c)
    {
        data[0] = c(0);
        data[1] = c(1);
        data[2] = c(2);
    }

    type &operator()(int c)
    {
        return data[c];
    }

    type operator()(int c) const
    {
        return data[c];
    }

    type data[3];
    // #pragma HLS ARRAY_PARTITION variable=data complete dim=0
};

template <typename type>
struct mat6;

template <typename type>
struct vec6
{
    vec6()
    {
    }

    vec6(type x, type y, type z, type a, type b, type c)
    {
        data[0] = x;
        data[1] = y;
        data[2] = z;
        data[3] = a;
        data[4] = b;
        data[5] = c;
    }

    void zero()
    {
        data[0] = 0.0;
        data[1] = 0.0;
        data[2] = 0.0;
        data[3] = 0.0;
        data[4] = 0.0;
        data[5] = 0.0;
    }

    type dot(vec6<type> b)
    {
        type result;

    vec6_dot_loop:
        for (int x = 0; x < 6; x++)
        {
            result += data[x] * b(x);
        }

        // result = data[0] * b(0) + data[1] * b(1) + data[2] * b(2) + data[3] * b(3) + data[4] * b(4) + data[5] * b(5);
        return result;
    }

    vec6<type> operator+(vec6<type> c)
    {
        vec6<type> result;
        result(0) = data[0] + c(0);
        result(1) = data[1] + c(1);
        result(2) = data[2] + c(2);
        result(3) = data[3] + c(3);
        result(4) = data[4] + c(4);
        result(5) = data[5] + c(5);
        return result;
    }

    template <typename type2>
    vec6 operator/(type2 c)
    {
        vec6 result;
        result(0) = data[0] / c;
        result(1) = data[1] / c;
        result(2) = data[2] / c;
        result(3) = data[3] / c;
        result(4) = data[4] / c;
        result(5) = data[5] / c;
        return result;
    }

    template <typename type2>
    vec6 operator*(type2 c)
    {
        vec6 result;
        result(0) = data[0] * c;
        result(1) = data[1] * c;
        result(2) = data[2] * c;
        result(3) = data[3] * c;
        result(4) = data[4] * c;
        result(5) = data[5] * c;
        return result;
    }

    mat6<type> outer(vec6<type> a)
    {
        mat6<type> result;
    vec6_outer_x_loop:
        for (int y = 0; y < 6; y++)
        {
        vec6_outer_y_loop:
            for (int x = 0; x < 6; x++)
                result(y, x) = data[y] * a(x);
        }
        return result;
    }

    void operator=(vec6<type> c)
    {
        data[0] = c(0);
        data[1] = c(1);
        data[2] = c(2);
        data[3] = c(3);
        data[4] = c(4);
        data[5] = c(5);
    }

    type &operator()(int c)
    {
        return data[c];
    }

    type operator()(int c) const
    {
        return data[c];
    }

    type data[6];
};

template <typename type>
struct mat2
{
    mat2()
    {
    }

    mat2(type d00, type d01, type d10, type d11)
    {
        data[0][0] = d00;
        data[0][1] = d01;
        data[1][0] = d10;
        data[1][1] = d11;
    }

    void zero()
    {
    mat2_zero_y_loop:
        for (int y = 0; y < 2; y++)
        {
        mat2_zero_x_loop:
            for (int x = 0; x < 2; x++)
            {
                data[y][x] = 0.0;
            }
        }
    }

    void identity()
    {
        data[0][0] = 1.0;
        data[0][1] = 0.0;
        data[1][0] = 0.0;
        data[1][1] = 1.0;
    }

    mat2 transpose()
    {
        mat2<type> result;
        result(0, 0) = data[0][0];
        result(0, 1) = data[1][0];
        result(1, 0) = data[0][1];
        result(1, 1) = data[1][1];
        return result;
    }

    /*
        mat3 inverse()
        {
        double determinant =    +A(0,0)*(A(1,1)*A(2,2)-A(2,1)*A(1,2))
                            -A(0,1)*(A(1,0)*A(2,2)-A(1,2)*A(2,0))
                            +A(0,2)*(A(1,0)*A(2,1)-A(1,1)*A(2,0));
    double invdet = 1/determinant;
    result(0,0) =  (A(1,1)*A(2,2)-A(2,1)*A(1,2))*invdet;
    result(1,0) = -(A(0,1)*A(2,2)-A(0,2)*A(2,1))*invdet;
    result(2,0) =  (A(0,1)*A(1,2)-A(0,2)*A(1,1))*invdet;
    result(0,1) = -(A(1,0)*A(2,2)-A(1,2)*A(2,0))*invdet;
    result(1,1) =  (A(0,0)*A(2,2)-A(0,2)*A(2,0))*invdet;
    result(2,1) = -(A(0,0)*A(1,2)-A(1,0)*A(0,2))*invdet;
    result(0,2) =  (A(1,0)*A(2,1)-A(2,0)*A(1,1))*invdet;
    result(1,2) = -(A(0,0)*A(2,1)-A(2,0)*A(0,1))*invdet;
    result(2,2) =  (A(0,0)*A(1,1)-A(1,0)*A(0,1))*invdet;
        }
        */

    template <typename type2>
    mat2 operator/(type2 c)
    {
        mat2<type> result;

    mat2_div_y_loop:
        for (int y = 0; y < 2; y++)
        {
        mat2_div_x_loop:
            for (int x = 0; x < 2; x++)
            {
                result.data[y][x] = data[y][x] / c;
            }
        }

        return result;
    }

    template <typename type2>
    mat2 operator*(type2 c)
    {
        mat2<type> result;

    mat2_mult_y_loop:
        for (int y = 0; y < 2; y++)
        {
        mat2_mult_x_loop:
            for (int x = 0; x < 2; x++)
            {
                result.data[y][x] = data[y][x] * c;
            }
        }

        return result;
    }

    mat2 dot(mat2 c)
    {
        mat2<type> result;

    mat2_dot_y_loop:
        for (int y = 0; y < 2; y++)
        {
        mat2_dot_x_loop:
            for (int x = 0; x < 2; x++)
            {
                result.data[y][x] = data[y][0] * c(0, x) + data[y][1] * c(1, x);
            }
        }

        return result;
    }

    vec2<type> dot(vec2<type> c) const
    {
        vec2<type> result;

    mat2_dot_loop:
        for (int y = 0; y < 2; y++)
        {
            result(y) = data[y][0] * c(0) + data[y][1] * c(1) + data[y][2] * c(2);
        }

        return result;
    }

    type mul_v1(mat2<type> c)
    {
        // #pragma HLS INLINE
        type result = 0.0;

    mat2_mul_v1_y_loop:
        for (int y = 0; y < 2; y++)
        {
        mat2_mul_v1_x_loop:
            for (int x = 0; x < 2; x++)
            {
                result += data[y][x] * c(y, x);
            }
        }

        return result;
    }

    type mul_v2(mat2<type> c)
    {
        // #pragma HLS INLINE

        type mul[2][2];
        // #pragma HLS ARRAY_PARTITION variable = mul complete dim = 0

    mat2_mul_v2_y_loop:
        for (int y = 0; y < 2; y++)
        {
#pragma HLS unroll
        mat2_mul_v2_x_loop:
            for (int x = 0; x < 2; x++)
            {
#pragma HLS unroll
                mul[y][x] = data[y][x] * c(y, x);
            }
        }

        type sum_lvl1_0 = mul[0][0] + mul[1][0];
        type sum_lvl1_1 = mul[0][1] + mul[1][1];

        type res = sum_lvl1_0 + sum_lvl1_1;

        return res;
    }

    type getMax()
    {
        // #pragma HLS INLINE
        type max1 = hls::max(data[0][0], data[0][1]);
        type max2 = hls::max(data[1][0], data[1][1]);
        return hls::max(max1, max2);
        /*
        mat2_max_y_loop:
        for(int y = 0; y < 2; y++)
        {
            mat2_max_x_loop:
            for(int x = 0; x < 2; x++)
            {
                if(data[y][x] > val)
                    val = data[y][x];
            }
        }
        return val;
        */
    }

    type &operator()(int b, int c)
    {
        return data[b][c];
    }

    type operator()(int b, int c) const
    {
        return data[b][c];
    }

    type data[2][2];
};

template <typename type>
struct mat3
{
    mat3()
    {
        // #pragma HLS ARRAY_PARTITION variable = data dim = 0 type = complete
    }

    mat3(const type d[3 * 3])
    {
        data[0][0] = d[0];
        data[0][1] = d[1];
        data[0][2] = d[2];
        data[1][0] = d[3];
        data[1][1] = d[4];
        data[1][2] = d[5];
        data[2][0] = d[6];
        data[2][1] = d[7];
        data[2][2] = d[8];
    }

    void zero()
    {
    mat3_zero_y_loop:
        for (int y = 0; y < 3; y++)
        {
        mat3_zero_x_loop:
            for (int x = 0; x < 3; x++)
            {
                data[y][x] = 0.0;
            }
        }
    }

    void identity()
    {
        zero();
        data[0][0] = 1.0;
        data[1][1] = 1.0;
        data[2][2] = 1.0;
    }

    mat3 transpose()
    {
        mat3<type> result;
        result(0, 0) = data[0][0];
        result(0, 1) = data[1][0];
        result(0, 2) = data[2][0];
        result(1, 0) = data[0][1];
        result(1, 1) = data[1][1];
        result(1, 2) = data[2][1];
        result(2, 0) = data[0][2];
        result(2, 1) = data[1][2];
        result(2, 2) = data[2][2];
        return result;
    }

    /*
        mat3 inverse()
        {
        double determinant =    +A(0,0)*(A(1,1)*A(2,2)-A(2,1)*A(1,2))
                            -A(0,1)*(A(1,0)*A(2,2)-A(1,2)*A(2,0))
                            +A(0,2)*(A(1,0)*A(2,1)-A(1,1)*A(2,0));
    double invdet = 1/determinant;
    result(0,0) =  (A(1,1)*A(2,2)-A(2,1)*A(1,2))*invdet;
    result(1,0) = -(A(0,1)*A(2,2)-A(0,2)*A(2,1))*invdet;
    result(2,0) =  (A(0,1)*A(1,2)-A(0,2)*A(1,1))*invdet;
    result(0,1) = -(A(1,0)*A(2,2)-A(1,2)*A(2,0))*invdet;
    result(1,1) =  (A(0,0)*A(2,2)-A(0,2)*A(2,0))*invdet;
    result(2,1) = -(A(0,0)*A(1,2)-A(1,0)*A(0,2))*invdet;
    result(0,2) =  (A(1,0)*A(2,1)-A(2,0)*A(1,1))*invdet;
    result(1,2) = -(A(0,0)*A(2,1)-A(2,0)*A(0,1))*invdet;
    result(2,2) =  (A(0,0)*A(1,1)-A(1,0)*A(0,1))*invdet;
        }
        */

    template <typename type2>
    mat3 operator/(type2 c)
    {
        mat3<type> result;

    mat3_div_y_loop:
        for (int y = 0; y < 3; y++)
        {
        mat3_div_x_loop:
            for (int x = 0; x < 3; x++)
            {
                result.data[y][x] = data[y][x] / c;
            }
        }

        return result;
    }

    template <typename type2>
    mat3 operator*(type2 c)
    {
        mat3<type> result;

    mat3_mult_y_loop:
        for (int y = 0; y < 3; y++)
        {
        mat3_mult_x_loop:
            for (int x = 0; x < 3; x++)
            {
                result.data[y][x] = data[y][x] * c;
            }
        }

        return result;
    }

    mat3 dot(mat3 c)
    {
        mat3<type> result;

    mat3_dot_y_loop:
        for (int y = 0; y < 3; y++)
        {
        mat3_dot_x_loop:
            for (int x = 0; x < 3; x++)
            {
                result.data[y][x] = data[y][0] * c(0, x) + data[y][1] * c(1, x) + data[y][2] * c(2, x);
            }
        }

        return result;
    }

    vec3<type> dot(vec3<type> c) const
    {
        vec3<type> result;

    mat3_dot_loop:
        for (int y = 0; y < 3; y++)
        {
            result(y) = data[y][0] * c(0) + data[y][1] * c(1) + data[y][2] * c(2);
        }

        return result;
    }

    type mul_v1(mat3<type> c)
    {
        // #pragma HLS INLINE
        type result = 0.0;

    mat3_mul_v1_y_loop:
        for (int y = 0; y < 3; y++)
        {
        mat3_mul_v1_x_loop:
            for (int x = 0; x < 3; x++)
            {
                result += data[y][x] * c(y, x);
            }
        }

        return result;
    }

    template <typename output_type, typename input_type>
    output_type mul_v2(mat3<input_type> c)
    {
        // #pragma HLS INLINE

        output_type mul[3][3];
        // #pragma HLS ARRAY_PARTITION variable = mul complete dim = 0

    mat3_mul_v2_y_loop:
        for (int y = 0; y < 3; y++)
        {
#pragma HLS unroll
        mat3_mul_v2_x_loop:
            for (int x = 0; x < 3; x++)
            {
#pragma HLS unroll
                mul[y][x] = output_type(data[y][x]) * output_type(c(y, x));
            }
        }

        output_type sum_lvl1_0 = mul[0][0] + mul[1][0];
        output_type sum_lvl1_1 = mul[0][1] + mul[1][1];
        output_type sum_lvl1_2 = mul[0][2] + mul[1][2];

        output_type sum_lvl2_0 = mul[2][0] + sum_lvl1_0;
        output_type sum_lvl2_1 = mul[2][1] + sum_lvl1_1;
        output_type sum_lvl2_2 = mul[2][2] + sum_lvl1_2;

        output_type sum_lvl3_0 = sum_lvl2_0 + sum_lvl2_1;

        output_type res = sum_lvl3_0 + sum_lvl2_2;

        return res;
    }

    type &operator()(int b, int c)
    {
        return data[b][c];
    }

    type operator()(int b, int c) const
    {
        return data[b][c];
    }

    type data[3][3];
};

template <typename type>
struct mat6
{
    mat6()
    {
        // #pragma HLS ARRAY_PARTITION variable=data complete dim=0
    }

    void zero()
    {
    mat6_zero_y_loop:
        for (int y = 0; y < 6; y++)
        {
        mat6_zero_x_loop:
            for (int x = 0; x < 6; x++)
            {
                data[y][x] = 0.0;
            }
        }
    }

    void identity()
    {
    mat6_identity_x_loop:
        for (int y = 0; y < 6; y++)
        {
        mat6_identy_y_loop:
            for (int x = 0; x < 6; x++)
            {
                if (x == y)
                    data[y][x] = 1.0;
                else
                    data[y][x] = 0.0;
            }
        }
    }

    template <typename type2>
    mat6 operator/(type2 c)
    {
        mat6<type> result;
    mat6_div_y_loop:
        for (int y = 0; y < 6; y++)
        {
        mat6_div_x_loop:
            for (int x = 0; x < 6; x++)
            {
                result(y, x) = data[y][x] / c;
            }
        }
        return result;
    }

    template <typename type2>
    mat6 operator*(type2 c)
    {
        mat6<type> result;

    mat6_mul_y_loop:
        for (int y = 0; y < 6; y++)
        {
        mat6_mul_x_loop:
            for (int x = 0; x < 6; x++)
            {
                result(y, x) = data[y][x] * c;
            }
        }
        return result;
    }

    mat6 operator+(mat6 c)
    {
        mat6<type> result;
    mat6_plus_x:
        for (int y = 0; y < 6; y++)
        {
        mat6_plus_y:
            for (int x = 0; x < 6; x++)
            {
                result(y, x) = data[y][x] + c(y, x);
            }
        }
        return result;
    }

    mat6 dot(mat6 c)
    {
        mat6<type> result;

    mat6_dot_y_loop:
        for (int y = 0; y < 6; y++)
        {
        mat6_dot_x_loop:
            for (int x = 0; x < 6; x++)
            {
            mat6_dot_z_loop:
                for (int z = 0; z < 6; z++)
                    result(y, x) += data[y][z] * c(z, y);
            }
        }

        return result;
    }

    vec6<type> dot(vec6<type> c)
    {
        vec6<type> result;
    mat6_dot_y_loop:
        for (int y = 0; y < 6; y++)
        {
        mat6_dot_x_loop:
            for (int x = 0; x < 6; x++)
            {
                result(y) += data[y][x] * c(x);
            }
        }
        return result;
    }

    /*
    void operator=(mat6_fpga<type> c)
    {
    mat6_set_x:
        for (int i = 0; i < 6; i++)
        {
        mat6_set_y:
            for (int j = 0; j < 6; j++)
            {
                this->operator()(i, j) = c(i, j);
            }
        }
    }
    */

    type &operator()(int b, int c)
    {
        return data[b][c];
    }

    type operator()(int b, int c) const
    {
        return data[b][c];
    }

    type data[6][6];
};

template <typename type>
struct SO3
{
    SO3()
    {
        identity();
    }

    /// Constructor from a normalized quaternion
    inline SO3(type qw, type qx, type qy, type qz)
    {
        fromQuaternion(qw, qx, qy, qz);
    }

    SO3(mat3<type> mat)
    {
        matrix = mat;
    }

    /// Construct from C arrays
    /// r is rotation matrix row major
    SO3(type *r)
    {
        matrix(0, 0) = r[0];
        matrix(0, 1) = r[1];
        matrix(0, 2) = r[2];
        matrix(1, 0) = r[3];
        matrix(1, 1) = r[4];
        matrix(1, 2) = r[5];
        matrix(2, 0) = r[6];
        matrix(2, 1) = r[7];
        matrix(2, 2) = r[8];
    }

    void fromQuaternion(type qw, type qx, type qy, type qz)
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
    inline SE3(cv::Mat_<float> r, cv::vec3f t)
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
    type s = hls::sin(angle);
    type c = hls::cos(angle);
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
    type s = hls::sin(angle);
    type c = hls::cos(angle);

    mat3<type> mat = (s / angle) * mat3<type>::identity() +
                     (1 - s / angle) * axis.outer(axis) +
                     ((1 - c) / angle) * wedge(axis);

    return SO3<type>(mat);
}

template <typename type>
struct SE3
{
    SE3()
    {
        identity();
    }

    /// Constructor from a normalized quaternion and a translation vector
    SE3(type qw, type qx, type qy, type qz, type tx, type ty, type tz)
    {
        rotation.fromQuaternion(qw, qx, qy, qz);
        translation.set(tx, ty, tz);
    }

    /// Construct from C arrays
    /// r is rotation matrix row major
    /// t is the translation vector (x y z)
    SE3(type *r, type *t)
    {
        rotation(0, 0) = r[0];
        rotation(0, 1) = r[1];
        rotation(0, 2) = r[2];

        rotation(1, 0) = r[3];
        rotation(1, 1) = r[4];
        rotation(1, 2) = r[5];

        rotation(2, 0) = r[6];
        rotation(2, 1) = r[7];
        rotation(2, 2) = r[8];

        translation(0) = t[0];
        translation(1) = t[1];
        translation(2) = t[2];
    }

    SE3(SO3<type> _rotation, vec3<type> _translation)
    {
        rotation = _rotation;
        translation = _translation;
    }

    /*
    inline SE3(cv::Mat_<float> r, cv::vec3f t)
    {
        data(0,0)=r(0,0); data(0,1)=r(0,1); data(0,2)=r(0,2); data(0,3)=t.x;
        data(1,0)=r(1,0); data(1,1)=r(1,1); data(1,2)=r(1,2); data(1,3)=t.y;
        data(2,0)=r(2,0); data(2,1)=r(2,1); data(2,2)=r(2,2); data(2,3)=t.z;
    }
    */

    void identity()
    {
        rotation.identity();
        translation.set(0.0, 0.0, 0.0);
    }

    SE3<type> inv() const
    {
        SE3<type> result;
        result.rotation = rotation.inv();
        result.translation = -result.rotation.dot(translation);
        return result;
    }

    /*
        inline type this->operator()()(int r, int c) const
        {
            return data[r][c];
        }

        inline type &this->operator()()(int r, int c)
        {
            return data[r][c];
        }
        */

    SE3<type> dot(const SE3<type> &rhs)
    {
        SE3<type> result;
        result.rotation = rotation.dot(rhs.rotation);
        result.translation = rotation.dot(rhs.translation) + translation;
        return result;
    }

    vec3<type> dot(const vec3<type> &rhs)
    {
        vec3<type> result = rotation.dot(rhs) + translation;
        return result;
    }
    /*
    void operator=(SE3<type> c)
    {
        rotation = c.rotation;
        translation = c.translation;
    }
    */

    SO3<type> rotation;
    vec3<type> translation;
};

template <typename type>
SE3<type> exp(vec6<type> xi)
{
    vec3<type> rho(xi(0), xi(1), xi(2));
    vec3<type> phi(xi(3), xi(4), xi(5));

    mat3<type> rotation = exp(phi);
    vec3<type> translation = left_jacobian(phi).dot(rho);

    return SE3<type>(rotation, translation);
}

struct HG
{
    HG()
    {
        // #pragma HLS ARRAY_PARTITION variable=J complete
        // #pragma HLS ARRAY_PARTITION variable=H complete dim=0

        // J.zero();
        // H.zero();
        // mse = 0;
        // count = 0;
    }

    void zero()
    {
        G.zero();
        H.zero();
        mse = 0;
        count = 0;
    }

    HG operator+(HG c)
    {
        HG result;
        result.G = G + c.G;
        result.H = H + c.H;
        result.mse = mse + c.mse;
        result.count = count + c.count;
        return result;
    }

    void operator=(HG c)
    {
        G = c.G;
        H = c.H;
        mse = c.mse;
        count = c.count;
    }

    vec6<float> G;
    mat6<float> H;
    float mse;
    int count;
};

template <typename type, int size>
class shift_register
{
public:
    shift_register()
    {
    }

    type shift_down(type val_in)
    {
        // #pragma HLS INLINE

        type val_out = data[0];

    shift_down_loop:
        for (int i = 0; i < size - 1; i++)
        {
            // #pragma HLS UNROLL

            data[i] = data[i + 1];
        }

        data[size - 1] = val_in;

        return val_out;
    }

private:
    type data[size];
};

template <typename type, int size>
class shift_mat3
{
public:
    shift_mat3()
    {
    }

    void shift_down(type val)
    {
        type m20 = matrix(2, 0);
        matrix(2, 0) = matrix(2, 1);
        matrix(2, 1) = matrix(2, 2);
        matrix(2, 2) = val;

        type line2_val = line2.shift_down(m20);

        type m10 = matrix(1, 0);
        matrix(1, 0) = matrix(1, 1);
        matrix(1, 1) = matrix(1, 2);
        matrix(1, 2) = line2_val;

        type line1_val = line1.shift_down(m10);

        // type m00 = matrix(0, 0);
        matrix(0, 0) = matrix(0, 1);
        matrix(0, 1) = matrix(0, 2);
        matrix(0, 2) = line1_val;
    }

    mat3<type> getMat()
    {
        return matrix;
    }

private:
    shift_register<type, size - 3> line1;
    shift_register<type, size - 3> line2;
    mat3<type> matrix;
};

template <typename type, int size>
class shift_mat2
{
public:
    shift_mat2()
    {
    }

    void shift_down(type val)
    {
        type m10 = matrix(1, 0);
        matrix(1, 0) = matrix(1, 1);
        matrix(1, 1) = val;

        type line_val = line.shift_down(m10);

        // type m10 = matrix(1, 0);
        matrix(0, 0) = matrix(0, 1);
        matrix(0, 1) = line_val;
    }

    mat2<type> getMat()
    {
        return matrix;
    }

private:
    shift_register<type, size - 2> line;
    mat2<type> matrix;
};
