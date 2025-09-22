#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "linalg/linalg.h"

// Declare the top-level function to be tested
extern "C" void linalg_so3exp_top(const double phi[3], double R[9]);

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

// Helper function to test so3Exp with a given axis-angle vector
int test_so3exp(const linalg::Vec3<double> &phi, const std::string &test_name) {
    int errors = 0;
    double phi_array[3] = {phi(0), phi(1), phi(2)};
    double R_array[9] = {0};
    
    // Call the function under test
    linalg_so3exp_top(phi_array, R_array);
    
    // Compute expected result using the reference implementation
    linalg::SO3<double> R_expected = linalg::so3Exp(phi);
    linalg::Mat3<double> R_expected_mat = R_expected.matrix();
    
    // Eigen implementation for comparison
    Eigen::Vector3d phi_e(phi(0), phi(1), phi(2));
    double theta = phi_e.norm();
    Eigen::Matrix3d R_eigen = Eigen::Matrix3d::Identity();
    if (theta > 1e-12) {
        Eigen::AngleAxisd aa(theta, phi_e.normalized());
        R_eigen = aa.toRotationMatrix();
    }
    
    // Check results against linalg reference
    double maxe_linalg = 0.0;
    double maxe_eigen = 0.0;
    // Compare with linalg reference
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            errors += check_rel_error(
                test_name + " [linalg] R[" + std::to_string(r) + "][" + std::to_string(c) + "]",
                R_expected_mat(r, c), 
                R_array[r * 3 + c], 
                maxe_linalg
            );
        }
    }
    
    // Compare with Eigen
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            errors += check_rel_error(
                test_name + " [Eigen] R[" + std::to_string(r) + "][" + std::to_string(c) + "]",
                R_eigen(r, c),
                R_array[r * 3 + c],
                maxe_eigen
            );
        }
    }
    
    // Use the maximum error from both comparisons
    double maxe = std::max(maxe_linalg, maxe_eigen);
    
    if (errors == 0) {
        std::cout << "PASS: " << test_name << " (max rel error = " << maxe << ")" << std::endl;
    } else {
        std::cerr << "FAIL: " << test_name << " - " << errors << " mismatches (max rel error = " << maxe << ")" << std::endl;
    }
    
    return errors;
}

int main() {
    int total_errors = 0;
    
    // Test 1: Zero rotation (identity)
    {
        linalg::Vec3<double> phi(0.0, 0.0, 0.0);
        total_errors += test_so3exp(phi, "Zero rotation");
    }
    
    // Test 2: Small rotation around X axis
    {
        linalg::Vec3<double> phi(0.1, 0.0, 0.0);
        total_errors += test_so3exp(phi, "Small X rotation");
    }
    
    // Test 3: Small rotation around Y axis
    {
        linalg::Vec3<double> phi(0.0, 0.2, 0.0);
        total_errors += test_so3exp(phi, "Small Y rotation");
    }
    
    // Test 4: Small rotation around Z axis
    {
        linalg::Vec3<double> phi(0.0, 0.0, 0.3);
        total_errors += test_so3exp(phi, "Small Z rotation");
    }
    
    // Test 5: Medium rotation (multiple axes)
    {
        linalg::Vec3<double> phi(0.5, -0.3, 0.4);
        total_errors += test_so3exp(phi, "Medium rotation (multi-axis)");
    }
    
    // Test 6: Larger rotation (multiple axes)
    {
        linalg::Vec3<double> phi(1.2, -0.8, 0.9);
        total_errors += test_so3exp(phi, "Larger rotation (multi-axis)");
    }
    
    // Test 7: Edge case - very small rotation (near zero)
    {
        linalg::Vec3<double> phi(1e-8, -2e-8, 3e-8);
        total_errors += test_so3exp(phi, "Tiny rotation");
    }
    
    if (total_errors == 0) {
        std::cout << "\nSUCCESS: All so3Exp tests passed!" << std::endl;
        return 0;
    } else {
        std::cerr << "\nFAILURE: " << total_errors << " mismatches found across all tests." << std::endl;
        return 1;
    }
}
