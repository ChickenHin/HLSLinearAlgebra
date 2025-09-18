#include "linalg/linalg.h"

extern "C" void top(const double q[4], const double v[3], double v_rot[3]) {
    // Convert input arrays to quaternion and vector
    linalg::Quaternion<double> quat(q[0], q[1], q[2], q[3]);  // w, x, y, z
    linalg::Vec3<double> vec(v[0], v[1], v[2]);
    
    // Rotate the vector using quaternion
    linalg::Vec3<double> vec_rot = quat * vec;
    
    // Copy the result to the output array
    v_rot[0] = vec_rot(0);
    v_rot[1] = vec_rot(1);
    v_rot[2] = vec_rot(2);
}
