cmake_minimum_required(VERSION 3.0)

project(graphics2)

add_definitions(-std=c++14)
add_executable(graphics2 graphics.cc)
target_include_directories(graphics2 PRIVATE /usr/include/cairomm-1.0)
