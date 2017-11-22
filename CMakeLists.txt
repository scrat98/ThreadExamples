cmake_minimum_required(VERSION 3.8)
project(lab4_ThreadsExamples)

set(CMAKE_CXX_STANDARD 14)

set(BOOST_ROOT C:/Boost/1.65.1)
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib)
set(BOOST_INCLUDEDIR ${BOOST_ROOT}/boost)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS thread chrono system)
include_directories(${Boost_INCLUDE_DIRS} src)
link_libraries(${Boost_LIBRARIES})

set(SOURCE_FILES
        main.cpp
        ThreadPool.hpp)

add_executable(lab4_ThreadsExamples ${SOURCE_FILES})