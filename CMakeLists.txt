cmake_minimum_required(VERSION 3.5)
project(timesync)

set(CMAKE_CXX_STANDARD 11)

# TimeSync library source files
set(TIMESYNC_LIB_SRCFILES
	inc/TimeSync/Counter.h
        src/TimeSync.cpp
	inc/TimeSync/TimeSync.h)

add_library(timesync SHARED ${TIMESYNC_LIB_SRCFILES})

set( HEADER_FILES inc/TimeSync/TimeSync.h inc/TimeSync/Counter.h )

set_target_properties(timesync PROPERTIES PUBLIC_HEADER "${HEADER_FILES}" )

install(TARGETS timesync
        EXPORT timesync
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
	PUBLIC_HEADER DESTINATION include/TimeSync
)

include_directories(inc)
add_executable(tests tests/tests.cpp)
target_link_libraries(tests timesync)
