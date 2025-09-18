set(INVERSE_HLS_COMMAND
    "source ${VITIS_SETTINGS_SCRIPT} && ${VITIS_EXECUTABLE} -s ${CMAKE_CURRENT_SOURCE_DIR}/run.py"
)
add_test(
    NAME Inverse_HLS_Test
    COMMAND bash -c "${INVERSE_HLS_COMMAND}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_tests_properties(Inverse_HLS_Test PROPERTIES
    TIMEOUT 1800
    LABELS "HLS;FPGA;LinAlg"
)

# Add custom target for HLS tests
add_custom_target(inverse_hls_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L "HLS"
    COMMENT "Running all HLS/FPGA tests"
)
