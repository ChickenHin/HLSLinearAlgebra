#include "linalg/linalg.h"
#include <iostream>

// Simple test function for HLS synthesis
void test_matrix_operations(double input[9], double output[9]) {
    #pragma HLS INTERFACE m_axi port=input offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=return
    
    // Create 3x3 matrix from input
    linalg::Mat3<double> mat;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            mat(i, j) = input[i * 3 + j];
        }
    }
    
    // Test matrix operations
    auto identity = linalg::Mat3<double>::Identity();
    auto result = mat * identity; // Should equal original matrix
    
    // Copy result to output
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            output[i * 3 + j] = result(i, j);
        }
    }
}

// Test vector operations
void test_vector_operations(double input[3], double output[3]) {
    #pragma HLS INTERFACE m_axi port=input offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=return
    
    linalg::Vec3<double> vec(input[0], input[1], input[2]);
    
    // Test norm calculation
    double norm = vec.norm();
    
    // Test cross product with unit vector
    linalg::Vec3<double> unit_x(1.0, 0.0, 0.0);
    auto cross_result = vec.cross(unit_x);
    
    output[0] = cross_result(0);
    output[1] = cross_result(1);
    output[2] = cross_result(2);
}

// Test SO(3) operations
void test_so3_operations(double quat[4], double output[9]) {
    #pragma HLS INTERFACE m_axi port=quat offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=return
    
    // Create SO(3) from quaternion
    linalg::Quaternion<double> q(quat[0], quat[1], quat[2], quat[3]);
    linalg::SO3<double> rotation(q);
    
    // Get rotation matrix
    auto rot_matrix = rotation.matrix();
    
    // Copy to output
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            output[i * 3 + j] = rot_matrix(i, j);
        }
    }
}
