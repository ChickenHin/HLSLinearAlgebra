#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <Eigen/Dense>
#include "linalg/linalg.h"

// Declare the top-level function to be tested
extern "C" void top(const double q[4], const double v[3], double v_rot[3]);

static bool check_rel_error(const std::string &name, double expected, double actual, double &max_err, double thresh = 1e-6)
{
    double err;
    // Use absolute error for numerically near-zero expected values to avoid inflated relative errors
    if (std::fabs(expected) > 1e-12)
        err = std::fabs(expected - actual) / std::fabs(expected);
    else
        err = std::fabs(expected - actual);
    if (err > max_err) max_err = err;
    if (err > thresh) {
        std::cerr << "ERROR: " << name << " mismatch: exp=" << std::setprecision(15) << expected
                  << " act=" << std::setprecision(15) << actual << " relerr=" << err << std::endl;
        return true;
    }
    return false;
}

// Helper function to test quaternion-vector rotation
int test_quaternion_rotate(const Eigen::Quaterniond &q, 
                          const Eigen::Vector3d &v,
                          const std::string &test_name) {
    int errors = 0;
    double q_array[4] = {q.w(), q.x(), q.y(), q.z()};
    double v_array[3] = {v(0), v(1), v(2)};
    double v_rot_array[3] = {0};
    
    // Call the function under test
    top(q_array, v_array, v_rot_array);
    
    Eigen::Vector3d eigen_rot = q * v;
    
    // Check results against Eigen
    double maxe_eigen = 0.0;
    for (int i = 0; i < 3; ++i) {
        errors += check_rel_error(
            test_name + " [Eigen] v_rot[" + std::to_string(i) + "]",
            eigen_rot(i),
            v_rot_array[i],
            maxe_eigen
        );
    }
    
    if (errors == 0) {
        std::cout << "PASS: " << test_name << " (max rel error = " << maxe_eigen << ")" << std::endl;
    } else {
        std::cerr << "FAIL: " << test_name << " - " << errors 
                  << " mismatches (max rel error = " << maxe_eigen << ")" << std::endl;
    }
    
    return errors;
}

// Helper function to create a quaternion from axis-angle
Eigen::Quaterniond quat_from_axis_angle(const Eigen::Vector3d &axis, double angle) {
    double half_angle = angle / 2.0;
    double s = std::sin(half_angle);
    return Eigen::Quaterniond(
        std::cos(half_angle),
        axis(0) * s,
        axis(1) * s,
        axis(2) * s
    );
}

int main() {
    int total_errors = 0;
    
    // Test vectors
    Eigen::Vector3d v1(1.0, 0.0, 0.0);  // Along X
    Eigen::Vector3d v2(0.0, 1.0, 0.0);  // Along Y
    Eigen::Vector3d v3(0.0, 0.0, 1.0);  // Along Z
    Eigen::Vector3d v4(1.0, 1.0, 1.0);  // Diagonal
    
    // Test 1: Identity rotation
    {
        Eigen::Quaterniond q(1.0, 0.0, 0.0, 0.0);  // Identity quaternion
        total_errors += test_quaternion_rotate(q, v1, "Identity rotation (X vector)");
        total_errors += test_quaternion_rotate(q, v2, "Identity rotation (Y vector)");
        total_errors += test_quaternion_rotate(q, v3, "Identity rotation (Z vector)");
        total_errors += test_quaternion_rotate(q, v4, "Identity rotation (diagonal vector)");
    }
    
    // Test 2: 90° rotation around X axis
    {
        Eigen::Vector3d axis(1.0, 0.0, 0.0);
        double angle = M_PI / 2.0;  // 90 degrees
        Eigen::Quaterniond q = quat_from_axis_angle(axis, angle);
        
        total_errors += test_quaternion_rotate(q, v1, "90° X rotation (X vector)");
        total_errors += test_quaternion_rotate(q, v2, "90° X rotation (Y vector)");
        total_errors += test_quaternion_rotate(q, v3, "90° X rotation (Z vector)");
        total_errors += test_quaternion_rotate(q, v4, "90° X rotation (diagonal vector)");
    }
    
    // Test 3: 45° rotation around Y axis
    {
        Eigen::Vector3d axis(0.0, 1.0, 0.0);
        double angle = M_PI / 4.0;  // 45 degrees
        Eigen::Quaterniond q = quat_from_axis_angle(axis, angle);
        
        total_errors += test_quaternion_rotate(q, v1, "45° Y rotation (X vector)");
        total_errors += test_quaternion_rotate(q, v2, "45° Y rotation (Y vector)");
        total_errors += test_quaternion_rotate(q, v3, "45° Y rotation (Z vector)");
        total_errors += test_quaternion_rotate(q, v4, "45° Y rotation (diagonal vector)");
    }
    
    // Test 4: 180° rotation around Z axis
    {
        Eigen::Vector3d axis(0.0, 0.0, 1.0);
        double angle = M_PI;  // 180 degrees
        Eigen::Quaterniond q = quat_from_axis_angle(axis, angle);
        
        total_errors += test_quaternion_rotate(q, v1, "180° Z rotation (X vector)");
        total_errors += test_quaternion_rotate(q, v2, "180° Z rotation (Y vector)");
        total_errors += test_quaternion_rotate(q, v3, "180° Z rotation (Z vector)");
        total_errors += test_quaternion_rotate(q, v4, "180° Z rotation (diagonal vector)");
    }
    
    // Test 5: Arbitrary rotation
    {
        Eigen::Vector3d axis(1.0, 1.0, 1.0);
        axis = axis * (1.0 / std::sqrt(3.0));  // Normalize
        double angle = 2.0 * M_PI / 3.0;  // 120 degrees
        Eigen::Quaterniond q = quat_from_axis_angle(axis, angle);
        
        total_errors += test_quaternion_rotate(q, v1, "120° diagonal rotation (X vector)");
        total_errors += test_quaternion_rotate(q, v2, "120° diagonal rotation (Y vector)");
        total_errors += test_quaternion_rotate(q, v3, "120° diagonal rotation (Z vector)");
        total_errors += test_quaternion_rotate(q, v4, "120° diagonal rotation (diagonal vector)");
    }
    
    // Test 6: Small angle rotation (near identity)
    {
        Eigen::Vector3d axis(0.0, 0.0, 1.0);
        double angle = 1e-6;  // Very small angle
        Eigen::Quaterniond q = quat_from_axis_angle(axis, angle);
        
        total_errors += test_quaternion_rotate(q, v1, "Tiny rotation (X vector)");
        total_errors += test_quaternion_rotate(q, v2, "Tiny rotation (Y vector)");
        total_errors += test_quaternion_rotate(q, v3, "Tiny rotation (Z vector)");
        total_errors += test_quaternion_rotate(q, v4, "Tiny rotation (diagonal vector)");
    }
    
    // Test 7: Non-unit quaternion (normalize before rotating)
    {
        Eigen::Quaterniond q(2.0, 0.0, 0.0, 0.0);  // Scaled identity
        q.normalize();
        total_errors += test_quaternion_rotate(q, v1, "Non-unit quaternion (X vector)");
        total_errors += test_quaternion_rotate(q, v4, "Non-unit quaternion (diagonal vector)");
    }
    
    if (total_errors == 0) {
        std::cout << "\nSUCCESS: All quaternion-vector rotation tests passed!" << std::endl;
        return 0;
    } else {
        std::cerr << "\nFAILURE: " << total_errors << " mismatches found across all tests." << std::endl;
        return 1;
    }
}
