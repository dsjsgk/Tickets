cmake_minimum_required(VERSION 3.16)
project(code)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -inline")
add_executable(code MAIN5.cpp)
