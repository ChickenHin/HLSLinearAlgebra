# =============================================================================
# CPU Tests using GoogleTest
# =============================================================================


# Linear Algebra CPU Tests (using general linalg library)
add_executable(linalg_cpu_test linalg_cpu_test.cpp)
target_link_libraries(linalg_cpu_test 
    gtest
    gtest_main
    linalg
)
gtest_discover_tests(linalg_cpu_test
    TEST_PREFIX "CPU."
    PROPERTIES TIMEOUT 300
)

# Comprehensive Linear Algebra Tests
add_executable(comprehensive_linalg_test comprehensive_linalg_test.cpp)
target_link_libraries(comprehensive_linalg_test 
    gtest
    gtest_main
    linalg
)
gtest_discover_tests(comprehensive_linalg_test
    TEST_PREFIX "Comprehensive."
    PROPERTIES TIMEOUT 300
)

# Comprehensive Linear Algebra Tests Part 2
add_executable(comprehensive_linalg_test2 comprehensive_linalg_test2.cpp)
target_link_libraries(comprehensive_linalg_test2 
    gtest
    gtest_main
    linalg
)
gtest_discover_tests(comprehensive_linalg_test2
    TEST_PREFIX "Comprehensive2."
    PROPERTIES TIMEOUT 300
)

# Numerical Precision Tests
add_executable(numerical_precision_test numerical_precision_test.cpp)
target_link_libraries(numerical_precision_test 
    gtest
    gtest_main
    linalg
)
gtest_discover_tests(numerical_precision_test
    TEST_PREFIX "Precision."
    PROPERTIES TIMEOUT 300
)

# Utility Tests (Shift Registers, Solvers, etc.)
add_executable(utility_test utility_test.cpp)
target_link_libraries(utility_test 
    gtest
    gtest_main
    linalg
)
gtest_discover_tests(utility_test
    TEST_PREFIX "Utility."
    PROPERTIES TIMEOUT 300
)

# Additional targeted tests for linalg functionality
add_executable(linalg_additional_test linalg_additional_test.cpp)
target_link_libraries(linalg_additional_test 
    gtest
    gtest_main
    linalg
)
gtest_discover_tests(linalg_additional_test
    TEST_PREFIX "Additional."
    PROPERTIES TIMEOUT 300
)

# Create a custom target to run all CPU tests
add_custom_target(run_cpu_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R "Basic|CPU|Comprehensive|Precision|Utility|Additional"
    DEPENDS linalg_cpu_test comprehensive_linalg_test comprehensive_linalg_test2 numerical_precision_test utility_test linalg_additional_test
    COMMENT "Running all CPU tests"
)
