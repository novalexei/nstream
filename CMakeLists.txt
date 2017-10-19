cmake_minimum_required(VERSION 3.8)
project(nstream)

#set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(CMAKE_CXX_STANDARD 17)

#set(SOURCE_FILES include/nova/io.h src/main.cpp src/sink.cpp)

include_directories( ${CMAKE_SOURCE_DIR}/include )

add_executable(sink include/nova/io.h src/sink.cpp)
add_executable(source include/nova/io.h src/source.cpp)
add_executable(sink_buffer include/nova/io.h src/sink_buffer.cpp)
add_executable(source_buffer include/nova/io.h src/source_buffer.cpp)
add_executable(device include/nova/io.h src/device.cpp)
