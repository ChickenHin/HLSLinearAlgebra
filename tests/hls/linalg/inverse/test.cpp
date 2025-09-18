#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include "linalg/linalg.h"

// Declare the top-level function to be tested
extern "C" void top(const double in_a[9], double out[9]);

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

int main()
{
    int errors = 0;
    double in_a[9] = {0};
    double out[9] = {0};
    
    // Test matrix
    double m[9] = {
        4.0, 7.0, 2.0,
        3.0, 6.0, 1.0,
        2.0, 5.0, 3.0
    };
    
    // Copy test matrix to input
    for (int i = 0; i < 9; ++i) in_a[i] = m[i];
    
    // Call the function under test
    top(in_a, out);
    
    // Build expected inverse using linalg in software as golden
    linalg::Mat3<double> M(m);
    linalg::Mat3<double> Minv = M.inverse();
    
    // Check results
    double maxe = 0.0;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            errors += check_rel_error("inv[" + std::to_string(r) + "," + std::to_string(c) + "]",
                                    Minv(r,c), out[r*3+c], maxe);
        }
    }

    if (errors == 0) {
        std::cout << "SUCCESS: All inverse tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: " << errors << " mismatches found." << std::endl;
        return 1;
    }
}
