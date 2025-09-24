# add_subdirectory(floatx)
# add_subdirectory(linalg)
add_subdirectory(posit)

# Add custom target for HLS tests
add_custom_target(run_hls_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L "HLS"
    COMMENT "Running all HLS/FPGA tests"
)
