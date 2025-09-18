#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "linalg/linalg.h"

// Declare the top-level function to be tested
extern "C" void linalg_se3exp_top(const double xi[6], double T[16]);

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

// Helper function to test se3Exp with a given twist vector
int test_se3exp(const linalg::Vec6<double> &xi, const std::string &test_name) {
    int errors = 0;
    double xi_array[6] = {xi(0), xi(1), xi(2), xi(3), xi(4), xi(5)};
    double T_array[16] = {0};
    
    // Call the function under test
    linalg_se3exp_top(xi_array, T_array);
    
    // Compute expected result using the reference implementation
    linalg::SE3<double> T_expected = linalg::se3Exp(xi);
    linalg::Mat4<double> T_expected_mat = T_expected.matrix();
    
    // Eigen implementation for comparison
    Eigen::Vector3d omega(xi(0), xi(1), xi(2));
    Eigen::Vector3d v(xi(3), xi(4), xi(5));
    
    Eigen::Matrix4d T_eigen = Eigen::Matrix4d::Identity();
    double theta = omega.norm();
    
    if (theta > 1e-10) {
        // Normalize the rotation part
        Eigen::Vector3d a = omega.normalized();
        
        // Skew-symmetric matrix of a
        Eigen::Matrix3d a_hat;
        a_hat << 0, -a(2), a(1),
                a(2), 0, -a(0),
                -a(1), a(0), 0;
        
        // Rodrigues' formula for rotation part
        Eigen::Matrix3d R = Eigen::Matrix3d::Identity() + 
                           sin(theta) * a_hat + 
                           (1 - cos(theta)) * a_hat * a_hat;
        
        // Compute translation part
        Eigen::Matrix3d V = Eigen::Matrix3d::Identity() + 
                           (1 - cos(theta)) / (theta * theta) * a_hat + 
                           (theta - sin(theta)) / (theta * theta * theta) * a_hat * a_hat;
        
        // Combine into SE(3) matrix
        T_eigen.block<3, 3>(0, 0) = R;
        T_eigen.block<3, 1>(0, 3) = V * v;
    } else {
        // For very small angles, use the first-order approximation
        Eigen::Matrix3d omega_hat;
        omega_hat << 0, -omega(2), omega(1),
                    omega(2), 0, -omega(0),
                    -omega(1), omega(0), 0;
        
        T_eigen.block<3, 3>(0, 0) = Eigen::Matrix3d::Identity() + omega_hat;
        T_eigen.block<3, 1>(0, 3) = v;
    }
    
    // Check results against linalg reference
    double maxe_linalg = 0.0;
    double maxe_eigen = 0.0;
    // Compare with linalg reference
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            errors += check_rel_error(
                test_name + " [linalg] T[" + std::to_string(r) + "][" + std::to_string(c) + "]",
                T_expected_mat(r, c), 
                T_array[r * 4 + c], 
                maxe_linalg
            );
        }
    }
    
    // Compare with Eigen
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            errors += check_rel_error(
                test_name + " [Eigen] T[" + std::to_string(r) + "][" + std::to_string(c) + "]",
                T_eigen(r, c),
                T_array[r * 4 + c],
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
    
    // Test 1: Zero twist (identity)
    {
        linalg::Vec6<double> xi(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
        total_errors += test_se3exp(xi, "Zero twist");
    }
    
    // Test 2: Pure translation
    {
        linalg::Vec6<double> xi(0.0, 0.0, 0.0, 0.5, -0.3, 0.2);
        total_errors += test_se3exp(xi, "Pure translation");
    }
    
    // Test 3: Pure rotation (around X axis)
    {
        linalg::Vec6<double> xi(0.5, 0.0, 0.0, 0.0, 0.0, 0.0);
        total_errors += test_se3exp(xi, "Pure rotation (X axis)");
    }
    
    // Test 4: Pure rotation (around Y axis)
    {
        linalg::Vec6<double> xi(0.0, 0.3, 0.0, 0.0, 0.0, 0.0);
        total_errors += test_se3exp(xi, "Pure rotation (Y axis)");
    }
    
    // Test 5: Pure rotation (around Z axis)
    {
        linalg::Vec6<double> xi(0.0, 0.0, 0.4, 0.0, 0.0, 0.0);
        total_errors += test_se3exp(xi, "Pure rotation (Z axis)");
    }
    
    // Test 6: General case (rotation + translation)
    {
        linalg::Vec6<double> xi(0.1, -0.2, 0.3, 0.5, -0.3, 0.2);
        total_errors += test_se3exp(xi, "General case (rotation + translation)");
    }
    
    // Test 7: Larger twist
    {
        linalg::Vec6<double> xi(0.8, -0.5, 0.6, 1.0, -0.7, 0.4);
        total_errors += test_se3exp(xi, "Larger twist");
    }
    
    // Test 8: Edge case - very small twist
    {
        linalg::Vec6<double> xi(1e-8, -2e-8, 3e-8, 4e-8, -5e-8, 6e-8);
        total_errors += test_se3exp(xi, "Tiny twist");
    }
    
    if (total_errors == 0) {
        std::cout << "\nSUCCESS: All se3Exp tests passed!" << std::endl;
        return 0;
    } else {
        std::cerr << "\nFAILURE: " << total_errors << " mismatches found across all tests." << std::endl;
        return 1;
    }
}
