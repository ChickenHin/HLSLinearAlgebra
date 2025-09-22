#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <string>

#include "../test_floatx_common.h"

extern "C" void top(double in_a, double in_b, double &out);

int main()
{
    std::cout << "--- Starting FloatX Addition Test ---" << std::endl;

    int error_count = 0;
    double max_error = 0.0;

    for (int i = 0; i < NUM_TEST_CASES; i++)
    {
        double a = FLOATX_TEST_VECTORS[i][0];
        double b = FLOATX_TEST_VECTORS[i][1];

        std::string a_str = format_double(a);
        std::string b_str = format_double(b);

        std::cout << "\nTest case " << (i + 1) << "/" << NUM_TEST_CASES
                  << ": " << a_str << " + " << b_str << std::endl;

        // Software (golden) result
        double sw_result = a + b;

        // Hardware (HLS) result
        double hw_result;
        top(a, b, hw_result);

        // Compare results
        if (check_floatx_error("Addition", sw_result, hw_result, max_error, a_str, b_str))
        {
            error_count++;
        }
    }

    std::cout << "\n--- Addition Test Summary ---" << std::endl;
    std::cout << "Total test cases: " << NUM_TEST_CASES << std::endl;
    std::cout << "Failed test cases: " << error_count << std::endl;
    std::cout << "Maximum relative error: " << max_error << std::endl;

    return (error_count > 0) ? 1 : 0;
}
