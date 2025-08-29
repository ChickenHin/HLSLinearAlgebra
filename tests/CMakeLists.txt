# =============================================================================
# Linear Algebra Library - Tests CMakeLists.txt
# =============================================================================

# =============================================================================
# CPU Tests using GoogleTest
# =============================================================================
if(BUILD_TESTS)
    # Basic CPU Tests (no dependencies)
    add_executable(basic_cpu_test cpu/basic_cpu_test.cpp)
    target_link_libraries(basic_cpu_test 
        gtest
        gtest_main
    )
    gtest_discover_tests(basic_cpu_test)
    
    # Linear Algebra CPU Tests (using general linalg library)
    add_executable(linalg_cpu_test cpu/linalg_cpu_test.cpp)
    target_link_libraries(linalg_cpu_test 
        gtest
        gtest_main
        linalg
    )
    gtest_discover_tests(linalg_cpu_test
        TEST_PREFIX "CPU."
        PROPERTIES TIMEOUT 300
    )
    
    # FloatX CPU Tests (disabled due to HLS compatibility issues)
    # add_executable(floatx_cpu_test FloatX_cpu_test.cpp)
    # target_link_libraries(floatx_cpu_test 
    #     gtest
    #     gtest_main
    # )
    # gtest_discover_tests(floatx_cpu_test)
    
    # Posit CPU Tests (disabled due to HLS compatibility issues)
    # add_executable(posit_cpu_test Posit_test.cpp)
    # target_link_libraries(posit_cpu_test 
    #     gtest
    #     gtest_main
    # )
    # gtest_discover_tests(posit_cpu_test)
    
    # RMSE Comparison Tests (disabled due to HLS compatibility issues)
    # add_executable(rmse_cpu_test rmse_test.cpp)
    # target_link_libraries(rmse_cpu_test 
    #     PRIVATE 
    #     hls_linalg
    #     gtest_main
    # )
    
    # Register tests with CTest
    gtest_discover_tests(basic_cpu_test
        TEST_PREFIX "Basic."
        PROPERTIES TIMEOUT 300
    )
    
    
    # Comprehensive Linear Algebra Tests
    add_executable(comprehensive_linalg_test cpu/comprehensive_linalg_test.cpp)
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
    add_executable(comprehensive_linalg_test2 cpu/comprehensive_linalg_test2.cpp)
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
    add_executable(numerical_precision_test cpu/numerical_precision_test.cpp)
    target_link_libraries(numerical_precision_test 
        gtest
        gtest_main
        linalg
    )
    gtest_discover_tests(numerical_precision_test
        TEST_PREFIX "Precision."
        PROPERTIES TIMEOUT 300
    )
    
    # Create a custom target to run all CPU tests
    add_custom_target(run_cpu_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R "Basic|CPU|Comprehensive|Precision"
        DEPENDS basic_cpu_test linalg_cpu_test comprehensive_linalg_test comprehensive_linalg_test2 numerical_precision_test
        COMMENT "Running all CPU tests"
    )
endif()

# =============================================================================
# HLS Tests using GoogleTest
# =============================================================================
if(BUILD_HLS_TESTS)
    
    # FloatX HLS Tests (disabled due to HLS compatibility issues)
    # add_executable(floatx_hls_test hls/FloatX_cpu_test.cpp)
    # target_link_libraries(floatx_hls_test 
    #     gtest
    #     gtest_main
    #     hls_numerics
    # )
    # gtest_discover_tests(floatx_hls_test)
    
    # Posit HLS Tests (disabled due to HLS compatibility issues)
    # add_executable(posit_hls_test hls/Posit_test.cpp)
    # target_link_libraries(posit_hls_test 
    #     gtest
    #     gtest_main
    #     hls_numerics
    # )
    # gtest_discover_tests(posit_hls_test)
endif()

# =============================================================================
# HLS/FPGA Synthesis Tests using Vitis
# =============================================================================
if(BUILD_HLS_TESTS AND VITIS_EXECUTABLE)
    # FloatX HLS Test
    add_test(
        NAME FloatX_HLS_Test
        COMMAND ${VITIS_EXECUTABLE} -s ${CMAKE_CURRENT_SOURCE_DIR}/run.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set_tests_properties(FloatX_HLS_Test PROPERTIES
        TIMEOUT 1800  # 30 minutes timeout for HLS synthesis
        LABELS "HLS;FPGA;FloatX"
    )
    
    # Posit HLS Test (template for future implementation)
    # Note: You'll need to create a run_posit.py script similar to run.py
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/run_posit.py)
        add_test(
            NAME Posit_HLS_Test
            COMMAND ${VITIS_EXECUTABLE} -s ${CMAKE_CURRENT_SOURCE_DIR}/run_posit.py
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        set_tests_properties(Posit_HLS_Test PROPERTIES
            TIMEOUT 1800
            LABELS "HLS;FPGA;Posit"
        )
    endif()
    
    # Add custom target for HLS tests
    add_custom_target(hls_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L "HLS"
        COMMENT "Running all HLS/FPGA tests"
    )
endif()

# =============================================================================
# Benchmarks (Optional)
# =============================================================================
if(BUILD_BENCHMARKS)
    # Performance benchmark executable
    add_executable(performance_benchmark rmse_test.cpp)
    target_link_libraries(performance_benchmark 
        PRIVATE 
        hls_linalg
    )
    target_compile_definitions(performance_benchmark PRIVATE BENCHMARK_MODE=1)
    
    add_custom_target(benchmarks
        COMMAND performance_benchmark
        DEPENDS performance_benchmark
        COMMENT "Running performance benchmarks"
    )
endif()

# =============================================================================
# Installation (Optional)
# =============================================================================
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    install(TARGETS linalg hls_numerics
        EXPORT linalgTargets
    )
    
    install(FILES
        include/linalg/linalg.h
        include/linalg/common.h
        include/linalg/shift_registers.h
        DESTINATION include/linalg
    )
    
    install(FILES
        include/hls_numerics/FloatX.h
        include/hls_numerics/Posit.h
        include/hls_numerics/common.h
        include/hls_numerics/hls_compat_ultra_simple.h
        DESTINATION include/hls_numerics
    )
endif()