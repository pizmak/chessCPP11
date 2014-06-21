cmake_minimum_required (VERSION 2.8) 
project (test) 
add_subdirectory(gtest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gtest/include)
include_directories (${CMAKE_SOURCE_DIR}/src)
add_executable(test test.cpp)
target_link_libraries (test gtest szachylib)


