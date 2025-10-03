#include "linalg/linalg.h"
#include <iostream>
#include <cmath>

// Function declarations (would normally be in header)
void test_matrix_operations(double input[9], double output[9]);
void test_vector_operations(double input[3], double output[3]);
void test_so3_operations(double quat[4], double output[9]);

int main() {
    std::cout << "=== HLS Linear Algebra Test Suite ===" << std::endl;
    
    // Test 1: Matrix operations
    std::cout << "\n1. Testing Matrix Operations..." << std::endl;
    double mat_input[9] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    double mat_output[9];
    
    test_matrix_operations(mat_input, mat_output);
    
    std::cout << "Input matrix:" << std::endl;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            std::cout << mat_input[i*3 + j] << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Output matrix (should be same as input):" << std::endl;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            std::cout << mat_output[i*3 + j] << " ";
        }
        std::cout << std::endl;
    }
    
    // Verify results
    bool mat_test_passed = true;
    for(int i = 0; i < 9; i++) {
        if(std::abs(mat_input[i] - mat_output[i]) > 1e-10) {
            mat_test_passed = false;
            break;
        }
    }
    std::cout << "Matrix test: " << (mat_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    // Test 2: Vector operations
    std::cout << "\n2. Testing Vector Operations..." << std::endl;
    double vec_input[3] = {0.0, 1.0, 0.0}; // y-axis unit vector
    double vec_output[3];
    
    test_vector_operations(vec_input, vec_output);
    
    std::cout << "Input vector: [" << vec_input[0] << ", " << vec_input[1] << ", " << vec_input[2] << "]" << std::endl;
    std::cout << "Cross product with x-axis: [" << vec_output[0] << ", " << vec_output[1] << ", " << vec_output[2] << "]" << std::endl;
    
    // Expected: y × x = -z = [0, 0, -1]
    bool vec_test_passed = (std::abs(vec_output[0] - 0.0) < 1e-10 && 
                           std::abs(vec_output[1] - 0.0) < 1e-10 && 
                           std::abs(vec_output[2] - (-1.0)) < 1e-10);
    std::cout << "Vector test: " << (vec_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    // Test 3: SO(3) operations
    std::cout << "\n3. Testing SO(3) Operations..." << std::endl;
    double quat_input[4] = {1.0, 0.0, 0.0, 0.0}; // Identity quaternion
    double rot_output[9];
    
    test_so3_operations(quat_input, rot_output);
    
    std::cout << "Input quaternion (identity): [" << quat_input[0] << ", " << quat_input[1] 
              << ", " << quat_input[2] << ", " << quat_input[3] << "]" << std::endl;
    std::cout << "Rotation matrix:" << std::endl;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            std::cout << rot_output[i*3 + j] << " ";
        }
        std::cout << std::endl;
    }
    
    // Verify identity matrix
    bool so3_test_passed = true;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            if(std::abs(rot_output[i*3 + j] - expected) > 1e-10) {
                so3_test_passed = false;
                break;
            }
        }
        if(!so3_test_passed) break;
    }
    std::cout << "SO(3) test: " << (so3_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    // Summary
    std::cout << "\n=== Test Summary ===" << std::endl;
    int passed_tests = (mat_test_passed ? 1 : 0) + (vec_test_passed ? 1 : 0) + (so3_test_passed ? 1 : 0);
    std::cout << "Tests passed: " << passed_tests << "/3" << std::endl;
    
    if(passed_tests == 3) {
        std::cout << "All HLS tests PASSED! ✅" << std::endl;
        return 0;
    } else {
        std::cout << "Some HLS tests FAILED! ❌" << std::endl;
        return 1;
    }
}
