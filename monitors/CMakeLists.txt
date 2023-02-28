cmake_minimum_required(VERSION 3.9)
project(monitory)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-pthread")

add_executable(monitor buffer.cpp buffer.h monitor.h)