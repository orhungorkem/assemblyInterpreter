cmake_minimum_required(VERSION 3.14)
project(assemblyInterpreter)

set(CMAKE_CXX_STANDARD 14)

add_executable(assemblyInterpreter main.cpp operate.cpp operate.h)