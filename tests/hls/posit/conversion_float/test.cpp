#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <string>

#include "../../test_common.h"

extern "C" void top(float in_a, float &out);

int main()
{
    std::cout << "--- Starting Posit float Test ---" << std::endl;

    int error_count = 0;
    double max_error = 0.0;

    for (int i = 0; i < NUM_TEST_CASES; i++)
    {
        float a = TEST_VECTORS[i][0];

        std::string a_str = format_double(a);

        std::cout << "\nTest case " << (i + 1) << "/" << NUM_TEST_CASES
                  << ": " << a_str << std::endl;

        // Software (golden) result
        float sw_result = a;

        // Hardware (HLS) result
        float hw_result;
        top(a, hw_result);

        // Compare results
        if (check_error("Addition", sw_result, hw_result, max_error, a_str, a_str))
        {
            error_count++;
        }
    }

    std::cout << "\n--- Conversion Test Summary ---" << std::endl;
    std::cout << "Total test cases: " << NUM_TEST_CASES << std::endl;
    std::cout << "Failed test cases: " << error_count << std::endl;
    std::cout << "Maximum relative error: " << max_error << std::endl;

    return (error_count > 0) ? 1 : 0;
}
