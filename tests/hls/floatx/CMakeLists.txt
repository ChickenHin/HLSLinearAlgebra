# FloatX test configuration

# Add the floatx_ops library
add_library(floatx_ops
    floatx_ops.cpp
)

# Set include directories
target_include_directories(floatx_ops PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Add test executables
add_executable(test_floatx_add test_floatx_add.cpp)
add_executable(test_floatx_sub test_floatx_sub.cpp)
add_executable(test_floatx_mul test_floatx_mul.cpp)
add_executable(test_floatx_div test_floatx_div.cpp)

# Link the floatx_ops library to all test executables
foreach(test_name IN ITEMS add sub mul div)
    target_link_libraries(test_floatx_${test_name} PRIVATE floatx_ops)
    
    # Add include directories
    target_include_directories(test_floatx_${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Add test
    add_test(NAME test_floatx_${test_name}
        COMMAND test_floatx_${test_name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach()

# Add the original test for backward compatibility
add_executable(floatX_test floatX_test.cpp)
target_link_libraries(floatX_test PRIVATE floatx_ops)
target_include_directories(floatX_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
add_test(NAME floatX_test
    COMMAND floatX_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add CPU tests if GoogleTest is available
if(TARGET gtest_main)
    add_executable(floatX_cpu_test floatX_cpu_test.cpp)
    target_link_libraries(floatX_cpu_test PRIVATE gtest_main)
    target_include_directories(floatX_cpu_test PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )
    add_test(NAME floatX_cpu_test
        COMMAND floatX_cpu_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()
