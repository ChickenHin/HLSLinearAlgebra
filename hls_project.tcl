# Vitis HLS TCL script for testing linear algebra library
# This script creates an HLS project and runs C simulation

# Set up project
open_project -reset hls_linalg_test
set_top test_matrix_operations

# Add source files
add_files test_hls_simple.cpp -cflags "-I./include -std=c++17"
add_files -tb test_hls_simple_tb.cpp -cflags "-I./include -std=c++17"

# Set solution and part
open_solution -reset "solution1"
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

# Run C simulation
csim_design

# Optional: Run synthesis (commented out for faster testing)
# csynth_design

# Optional: Run co-simulation (commented out for faster testing)  
# cosim_design

exit
