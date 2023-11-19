cmake_minimum_required(VERSION 3.10)
project(Processes_with_semaphore)

set (CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp)
add_executable(child child.cpp)
