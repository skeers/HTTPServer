cmake_minimum_required(VERSION 3.9)
project(HTTPServer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_executable(HTTPServer main.cpp squeue.h config.h config.cpp logger.h logger.cpp util.h util.cpp netctl.h netctl.cpp imp.h socketctl.h socketctl.cpp http.h http.cpp poller.h poller.cpp)