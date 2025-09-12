#include "linalg/linalg.h"
#include "hls_numerics/FloatX.h"
#include "hls_numerics/Posit.h"

void mat3_vec3_mul(int id, double m[9], double v[3], double &out_v0, double &out_v1, double &out_v2)
{
// HLS pragmas to define the interface for the hardware kernel
#pragma HLS INTERFACE s_axilite port = m bundle = control
#pragma HLS INTERFACE s_axilite port = v bundle = control
#pragma HLS INTERFACE s_axilite port = out_v bundle = control

    linalg::Mat3<double> mat_double(m);
    linalg::Vec3<double> vec_double(v[0], v[1], v[2]);
    linalg::Vec3<double> result_double = mat_double * vec_double;

    linalg::Mat3<float> mat_float(m);
    linalg::Vec3<float> vec_float(v[0], v[1], v[2]);
    linalg::Vec3<float> result_float = mat_float * vec_float;

    linalg::Mat3<FloatX<32, 8>> mat_floatx(m);
    linalg::Vec3<FloatX<32, 8>> vec_floatx(v[0], v[1], v[2]);
    linalg::Vec3<FloatX<32, 8>> result_floatx = mat_floatx * vec_floatx;

    linalg::Mat3<Posit<32, 3>> mat_posit(m);
    linalg::Vec3<Posit<32, 3>> vec_posit(v[0], v[1], v[2]);
    linalg::Vec3<Posit<32, 3>> result_posit = mat_posit * vec_posit;

    switch (id)
    {
    case 0:
        out_v0 = result_double(0);
        out_v1 = result_double(1);
        out_v2 = result_double(2);
        break;
    case 1:
        out_v0 = (double)result_float(0);
        out_v1 = (double)result_float(1);
        out_v2 = (double)result_float(2);
        break;
    case 2:
        out_v0 = (double)result_floatx(0);
        out_v1 = (double)result_floatx(1);
        out_v2 = (double)result_floatx(2);
        break;
    case 3:
        out_v0 = (double)result_posit(0);
        out_v1 = (double)result_posit(1);
        out_v2 = (double)result_posit(2);
        break;
    default:
        break;
    }
}
