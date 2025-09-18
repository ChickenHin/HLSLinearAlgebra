set(DET_HLS_COMMAND
    "source ${VITIS_SETTINGS_SCRIPT} && ${VITIS_EXECUTABLE} -s ${CMAKE_CURRENT_SOURCE_DIR}/run.py"
)
add_test(
    NAME Det_HLS_Test
    COMMAND bash -c "${DET_HLS_COMMAND}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_tests_properties(Det_HLS_Test PROPERTIES
    TIMEOUT 1800
    LABELS "HLS;FPGA;LinAlg"
)

# Add custom target for HLS tests
add_custom_target(determinant_hls_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L "HLS"
    COMMENT "Running all HLS/FPGA tests"
)
