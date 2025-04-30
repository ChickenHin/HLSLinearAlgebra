cmake_minimum_required(VERSION 3.14)

project(HLSLinearAlgebra)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0)

FetchContent_MakeAvailable(googletest)

include(CTest)
enable_testing()

add_subdirectory(src)
