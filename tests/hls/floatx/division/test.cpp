#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <string>

#include "../test_floatx_common.h"

extern "C" void top(double in_a, double in_b, double &out);

int main()
{
    std::cout << "--- Starting FloatX Division Test ---" << std::endl;

    int error_count = 0;
    double max_error = 0.0;

    for (int i = 0; i < NUM_TEST_CASES; i++)
    {
        double a = FLOATX_TEST_VECTORS[i][0];
        double b = FLOATX_TEST_VECTORS[i][1];

        // Skip division by zero cases in the main test loop
        if (b == 0.0)
            continue;

        std::string a_str = format_double(a);
        std::string b_str = format_double(b);

        std::cout << "\nTest case " << (i + 1) << "/" << NUM_TEST_CASES
                  << ": " << a_str << " / " << b_str << std::endl;

        // Software (golden) result
        double sw_result = a / b;

        // Hardware (HLS) result
        double hw_result;
        floatx_div(a, b, hw_result);

        // Compare results
        if (check_floatx_error("Division", sw_result, hw_result, max_error, a_str, b_str))
        {
            error_count++;
        }
    }

    std::cout << "\n--- Division Test Summary ---" << std::endl;
    std::cout << "Total test cases: " << (NUM_TEST_CASES - 1) << std::endl; // -1 for the division by zero case
    std::cout << "Failed test cases: " << error_count << std::endl;
    std::cout << "Maximum relative error: " << max_error << std::endl;

    return (error_count > 0) ? 1 : 0;
}
