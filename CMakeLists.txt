cmake_minimum_required(VERSION 3.30)
project(NetworkTools)

set(CMAKE_CXX_STANDARD 26)

add_executable(TEST_SERVER server_test.cpp)
add_executable(TEST_CLIENT client_test.cpp)

