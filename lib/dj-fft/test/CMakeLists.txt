cmake_minimum_required (VERSION 3.1)
set (CMAKE_CXX_STANDARD 11)

include_directories(..)

project (fft_test)
add_executable(fft_test test.cpp)
