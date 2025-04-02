include(GoogleTest)

add_test(
  NAME floatX
  COMMAND /tools/Xilinx/Vitis/2023.2/bin/vitis -s run.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})