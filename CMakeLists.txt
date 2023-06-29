cmake_minimum_required(VERSION 3.1...3.26)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    multi-transcoder
    VERSION 0.1
    LANGUAGES CXX)

file(GLOB SOURCES "src/*.cpp")
add_executable(multi-transcode ${SOURCES})
target_link_libraries(multi-transcode PRIVATE libvpl.so)
