cmake_minimum_required(VERSION 3.1...3.26)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    multi-transcoder
    VERSION 0.1
    LANGUAGES CXX)

# If you set any CMAKE_ variables, that can go here.
# (But usually don't do this, except maybe for C++ standard)

# Find packages go here.

# add_library(MyLibExample simple_lib.cpp simple_lib.hpp)

file(GLOB SOURCES "src/*.cpp")
add_executable(main ${SOURCES})
target_link_libraries(main PRIVATE libvpl.so)
