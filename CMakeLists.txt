cmake_minimum_required(VERSION 3.10)

project(Chip8Interpreter 
        VERSION 1.0
        DESCRIPTION "A CMake/C++ template project"
        LANGUAGES CXX
        )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)
add_subdirectory(apps)