# Find required packages for HLS (optional)
if(BUILD_HLS_TESTS)
    # Check for Vitis HLS
    if(NOT XILINX_VITIS_PATH AND DEFINED ENV{XILINX_VITIS})
        set(XILINX_VITIS_PATH $ENV{XILINX_VITIS} CACHE PATH "Path to Xilinx Vitis, initialized from ENV{XILINX_VITIS}")
    else()
        set(XILINX_VITIS_PATH "/tools/Xilinx/Vitis/2023.2/" CACHE PATH "Path to Xilinx Vitis, initialized to default")
    endif()

    set(XILINX_VITIS_PATH "${XILINX_VITIS_PATH}" CACHE PATH "Path to the Xilinx Vitis installation directory.")

    find_program(VITIS_EXECUTABLE vitis
        PATHS ${XILINX_VITIS_PATH}/bin
        NO_DEFAULT_PATH
    )

    if(NOT VITIS_EXECUTABLE)
        message(WARNING "Vitis executable not found at ${XILINX_VITIS_PATH}/bin. HLS tests will be disabled.")
        set(BUILD_HLS_TESTS OFF)
    else()
        message(STATUS "Found Vitis executable: ${VITIS_EXECUTABLE}")
    endif()

endif()

if(BUILD_TESTS)
  add_subdirectory(cpu)
endif()

if(BUILD_HLS_TESTS AND VITIS_EXECUTABLE)
  find_file(VITIS_SETTINGS_SCRIPT 
      NAMES settings64.sh
      PATHS ${XILINX_VITIS_PATH}
      NO_DEFAULT_PATH
      REQUIRED
  )
  message(STATUS "Found Vitis settings script: ${VITIS_SETTINGS_SCRIPT}")

  add_subdirectory(hls)
endif()