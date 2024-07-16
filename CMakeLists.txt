cmake_minimum_required(VERSION 3.10)
project(VirtualAsmMachine)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h")

foreach(file ${SOURCE_FILES})
    message(STATUS "Found file: ${file}")
endforeach()

add_executable(virtualAsmMachine ${SOURCE_FILES})
