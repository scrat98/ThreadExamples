cmake_minimum_required(VERSION 3.8)
project(lab4_ThreadExamples)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        main.cpp
        ThreadPool.hpp)

add_executable(lab4_ThreadExamples ${SOURCE_FILES})