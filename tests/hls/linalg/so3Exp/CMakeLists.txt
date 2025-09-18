# Add the so3Exp test executable
add_executable(linalg_so3exp_test
    linalg_so3exp_test.cpp
    linalg_so3exp_top.cpp
)

# Link against the linalg and Eigen3 libraries
target_link_libraries(linalg_so3exp_test PRIVATE 
    linalg
    Eigen3::Eigen
)

# Add include directories
target_include_directories(linalg_so3exp_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIRS}
)

# Add test
add_test(NAME linalg_so3exp_test
    COMMAND linalg_so3exp_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
