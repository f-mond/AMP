cmake_minimum_required(VERSION 3.15)
project(test)

link_directories(${PROJECT_SOURCE_DIR})

add_library(casn SHARED casn.cpp)
add_executable(test main.cpp casn.cpp)
target_link_libraries(test casn)

