#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include "linalg/linalg.h"

// Declare the top-level function to be tested
extern "C" void top(double in_0,
                    double in_1,
                    double in_2,
                    double in_3,
                    double in_4,
                    double in_5,
                    double in_6,
                    double in_7,
                    double in_8,
                    double &out_0,
                    double &out_1,
                    double &out_2,
                    double &out_3,
                    double &out_4,
                    double &out_5,
                    double &out_6,
                    double &out_7,
                    double &out_8);

static bool check_rel_error(const std::string &name, double expected, double actual, double &max_err, double thresh = 1e-6)
{
    double err;
    if (expected != 0.0)
        err = std::fabs(expected - actual) / std::fabs(expected);
    else
        err = std::fabs(expected - actual);
    if (err > max_err)
        max_err = err;
    if (err > thresh)
    {
        std::cerr << "ERROR: " << name << " mismatch: exp=" << std::setprecision(15) << expected
                  << " act=" << std::setprecision(15) << actual << " relerr=" << err << std::endl;
        return true;
    }
    return false;
}

int main()
{
    int errors = 0;

    // Test matrix
    double in_a[9] = {
        4.0, 7.0, 2.0,
        3.0, 6.0, 1.0,
        2.0, 5.0, 3.0};

    double out_hls[9] = {0};

    double out_ref[9] = {1.44444444, -1.22222222, -0.55555556,
                         -0.77777778, 0.88888889, 0.22222222,
                         0.33333333, -0.66666667, 0.33333333};

    // Call the function under test
    top(in_a[0], in_a[1], in_a[2], in_a[3], in_a[4], in_a[5], in_a[6], in_a[7], in_a[8],
        out_hls[0], out_hls[1], out_hls[2], out_hls[3], out_hls[4], out_hls[5], out_hls[6], out_hls[7], out_hls[8]);

    // Check results
    double maxe = 0.0;
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            errors += check_rel_error("inv[" + std::to_string(r) + "," + std::to_string(c) + "]",
                                      out_ref[r * 3 + c], out_hls[r * 3 + c], maxe);
        }
    }

    if (errors == 0)
    {
        std::cout << "SUCCESS: All inverse tests passed!" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "FAILURE: " << errors << " mismatches found." << std::endl;
        return 1;
    }
}
