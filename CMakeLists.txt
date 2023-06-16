cmake_minimum_required(VERSION 3.10)

# Project description
project(Chip8Interpreter 
        VERSION 1.0
        LANGUAGES CXX
        )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(OPENGL_INCLUDE_DIRS )
# Process the src and apps directories
add_subdirectory(src)
add_subdirectory(apps)

# Find OpenGL and GLUT packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Force the generation of a compile_commands.json file to provide autocompletion for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "" FORCE)

configure_file(${CMAKE_SOURCE_DIR}/resources/pong2.c8 ${CMAKE_BINARY_DIR}/resources/pong2.c8 COPYONLY)