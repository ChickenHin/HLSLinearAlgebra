#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "linalg/linalg.h"

// Declare the top-level function to be tested
extern "C" void linalg_so3leftjacobian_top(const double phi[3], double J[9]);

static bool check_rel_error(const std::string &name, double expected, double actual, double &max_err, double thresh = 1e-6)
{
    double err;
    if (expected != 0.0)
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

// Helper function to test so3LeftJacobian with a given axis-angle vector
int test_so3leftjacobian(const linalg::Vec3<double> &phi, const std::string &test_name) {
    int errors = 0;
    double phi_array[3] = {phi(0), phi(1), phi(2)};
    double J_array[9] = {0};
    
    // Call the function under test
    linalg_so3leftjacobian_top(phi_array, J_array);
    
    // Compute expected result using the reference implementation
    linalg::Mat3<double> J_expected = linalg::so3LeftJacobian(phi);
    
    // Eigen implementation for comparison
    Eigen::Matrix3d J_eigen = Eigen::Matrix3d::Identity();
    double phi_norm = phi.norm();
    if (phi_norm > 1e-10) {
        Eigen::Vector3d a = phi / phi_norm;
        double s = std::sin(phi_norm);
        double c = std::cos(phi_norm);
        double alpha = (1.0 - c) / (phi_norm * phi_norm);
        double beta = (phi_norm - s) / (phi_norm * phi_norm * phi_norm);
        
        // J = I - alpha * [a]x + beta * [a]x^2
        Eigen::Matrix3d ax;
        ax << 0, -a.z(), a.y(),
              a.z(), 0, -a.x(),
              -a.y(), a.x(), 0;
        J_eigen = Eigen::Matrix3d::Identity() - alpha * ax + beta * (ax * ax);
    }
    
    // Check results against linalg reference
    double maxe_linalg = 0.0;
    double maxe_eigen = 0.0;
    // Compare with linalg reference
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            errors += check_rel_error(
                test_name + " [linalg] J[" + std::to_string(r) + "][" + std::to_string(c) + "]",
                J_expected(r, c), 
                J_array[r * 3 + c], 
                maxe_linalg
            );
        }
    }
    
    // Compare with Eigen
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            errors += check_rel_error(
                test_name + " [Eigen] J[" + std::to_string(r) + "][" + std::to_string(c) + "]",
                J_eigen(r, c),
                J_array[r * 3 + c],
                maxe_eigen
            );
        }
    }
    
    // Use the maximum error from both comparisons
    double maxe = std::max(maxe_linalg, maxe_eigen);
    
    if (errors == 0) {
        std::cout << "PASS: " << test_name << " (max rel error = " << maxe << ")" << std::endl;
    } else {
        std::cerr << "FAIL: " << test_name << " - " << errors 
                  << " mismatches (max rel error = " << maxe << ")" << std::endl;
    }
    
    return errors;
}

int main() {
    int total_errors = 0;
    
    // Test 1: Zero rotation (identity)
    {
        linalg::Vec3<double> phi(0.0, 0.0, 0.0);
        total_errors += test_so3leftjacobian(phi, "Zero rotation");
    }
    
    // Test 2: Small rotation around X axis
    {
        linalg::Vec3<double> phi(0.1, 0.0, 0.0);
        total_errors += test_so3leftjacobian(phi, "Small X rotation");
    }
    
    // Test 3: Small rotation around Y axis
    {
        linalg::Vec3<double> phi(0.0, 0.2, 0.0);
        total_errors += test_so3leftjacobian(phi, "Small Y rotation");
    }
    
    // Test 4: Small rotation around Z axis
    {
        linalg::Vec3<double> phi(0.0, 0.0, 0.3);
        total_errors += test_so3leftjacobian(phi, "Small Z rotation");
    }
    
    // Test 5: Medium rotation (multiple axes)
    {
        linalg::Vec3<double> phi(0.5, -0.3, 0.4);
        total_errors += test_so3leftjacobian(phi, "Medium rotation (multi-axis)");
    }
    
    // Test 6: Larger rotation (multiple axes)
    {
        linalg::Vec3<double> phi(1.2, -0.8, 0.9);
        total_errors += test_so3leftjacobian(phi, "Larger rotation (multi-axis)");
    }
    
    // Test 7: Edge case - very small rotation (near zero)
    {
        linalg::Vec3<double> phi(1e-8, -2e-8, 3e-8);
        total_errors += test_so3leftjacobian(phi, "Tiny rotation");
    }
    
    if (total_errors == 0) {
        std::cout << "\nSUCCESS: All so3LeftJacobian tests passed!" << std::endl;
        return 0;
    } else {
        std::cerr << "\nFAILURE: " << total_errors << " mismatches found across all tests." << std::endl;
        return 1;
    }
}
