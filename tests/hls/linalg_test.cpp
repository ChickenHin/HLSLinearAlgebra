#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <string>
#include "linalg/linalg.h"

// Declare the top-level function to be tested
extern "C" void linalg_top(int id, const double in_a[16], const double in_b[16], double out[16]);

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

    // 0: determinant
    {
        double in_a[16] = {0};
        double in_b[16] = {0};
        double out[16] = {0};
        // Matrix (row-major)
        double m[9] = {
            1.0, 2.0, 3.0,
            0.0, 1.0, 4.0,
            5.0, 6.0, 0.0
        };
        for (int i = 0; i < 9; ++i) in_a[i] = m[i];
        linalg_top(0, in_a, in_b, out);
        // Golden determinant
        double det = m[0]*(m[4]*m[8]-m[5]*m[7]) - m[1]*(m[3]*m[8]-m[5]*m[6]) + m[2]*(m[3]*m[7]-m[4]*m[6]);
        double maxe = 0.0; errors += check_rel_error("det", det, out[0], maxe);
    }

    // 1: inverse
    {
        double in_a[16] = {0};
        double in_b[16] = {0};
        double out[16] = {0};
        double m[9] = {
            4.0, 7.0, 2.0,
            3.0, 6.0, 1.0,
            2.0, 5.0, 3.0
        };
        for (int i = 0; i < 9; ++i) in_a[i] = m[i];
        linalg_top(1, in_a, in_b, out);
        // Build inverse with linalg in software as golden (same math, but acceptable for regression)
        linalg::Mat3<double> M(m);
        linalg::Mat3<double> Minv = M.inverse();
        double maxe = 0.0;
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                errors += check_rel_error("inv[" + std::to_string(r) + "," + std::to_string(c) + "]",
                                          Minv(r,c), out[r*3+c], maxe);
    }

    // 2: so3Exp
    {
        double in_a[16] = {0};
        double in_b[16] = {0};
        double out[16] = {0};
        linalg::Vec3<double> phi(0.1, -0.2, 0.3);
        in_a[0] = phi(0); in_a[1] = phi(1); in_a[2] = phi(2);
        linalg_top(2, in_a, in_b, out);
        auto R = linalg::so3Exp(phi).matrix();
        double maxe = 0.0;
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                errors += check_rel_error("R[" + std::to_string(r) + "," + std::to_string(c) + "]",
                                          R(r,c), out[r*3+c], maxe, 1e-6);
    }

    // 3: so3LeftJacobian
    {
        double in_a[16] = {0};
        double in_b[16] = {0};
        double out[16] = {0};
        linalg::Vec3<double> phi(-0.3, 0.2, 0.1);
        in_a[0] = phi(0); in_a[1] = phi(1); in_a[2] = phi(2);
        linalg_top(3, in_a, in_b, out);
        auto J = linalg::so3LeftJacobian(phi);
        double maxe = 0.0;
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                errors += check_rel_error("J[" + std::to_string(r) + "," + std::to_string(c) + "]",
                                          J(r,c), out[r*3+c], maxe, 1e-6);
    }

    // 4: se3Exp
    {
        double in_a[16] = {0};
        double in_b[16] = {0};
        double out[16] = {0};
        linalg::Vec6<double> xi(0.1, -0.1, 0.2, 0.05, -0.02, 0.03);
        for (int i = 0; i < 6; ++i) in_a[i] = xi(i);
        linalg_top(4, in_a, in_b, out);
        auto T = linalg::se3Exp(xi).matrix();
        double maxe = 0.0;
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                errors += check_rel_error("T[" + std::to_string(r) + "," + std::to_string(c) + "]",
                                          T(r,c), out[r*4+c], maxe, 1e-6);
    }

    // 5: quaternion rotate vector
    {
        double in_a[16] = {0};
        double in_b[16] = {0};
        double out[16] = {0};
        linalg::Quaternion<double> q(0.9238795325, 0.0, 0.3826834324, 0.0); // 45deg about Y
        linalg::Vec3<double> v(1.0, 0.0, 0.0);
        in_a[0] = q.w(); in_a[1] = q.x(); in_a[2] = q.y(); in_a[3] = q.z();
        in_b[0] = v(0); in_b[1] = v(1); in_b[2] = v(2);
        linalg_top(5, in_a, in_b, out);
        auto vr = q * v;
        double maxe = 0.0;
        errors += check_rel_error("vr[0]", vr(0), out[0], maxe, 1e-6);
        errors += check_rel_error("vr[1]", vr(1), out[1], maxe, 1e-6);
        errors += check_rel_error("vr[2]", vr(2), out[2], maxe, 1e-6);
    }

    if (errors == 0) {
        std::cout << "\nSUCCESS: All linalg HLS tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\nFAILURE: " << errors << " mismatches found." << std::endl;
        return 1;
    }
}
