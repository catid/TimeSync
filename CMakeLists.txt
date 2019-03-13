cmake_minimum_required(VERSION 3.5)
project(timesync)

set(CMAKE_CXX_STANDARD 11)

# TimeSync library source files
set(TIMESYNC_LIB_SRCFILES
        Counter.h
        TimeSync.cpp
        TimeSync.h)

add_library(timesync SHARED ${TIMESYNC_LIB_SRCFILES})

add_executable(tests tests/tests.cpp)
target_link_libraries(tests timesync)
