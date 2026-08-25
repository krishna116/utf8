cmake_minimum_required(VERSION 4.0)

project(utf8 VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(testme
test.cpp
)
