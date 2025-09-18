#add_subdirectory(cpu)

find_file(VITIS_SETTINGS_SCRIPT 
    NAMES settings64.sh
    PATHS ${XILINX_VITIS_PATH}
    NO_DEFAULT_PATH
    REQUIRED
)
message(STATUS "Found Vitis settings script: ${VITIS_SETTINGS_SCRIPT}")

add_subdirectory(hls)