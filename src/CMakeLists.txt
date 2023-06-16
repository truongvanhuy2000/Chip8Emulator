# Get all the C++ files composing the render library
file(GLOB render_FILES ${CMAKE_CURRENT_SOURCE_DIR}/render/*.cpp)

# Get all the C++ files composing the chip8 library
file(GLOB chip8_FILES ${CMAKE_CURRENT_SOURCE_DIR}/chip8/*.cpp)

# Create a shared library using these files
add_library(render SHARED ${render_FILES})
add_library(chip8 SHARED ${chip8_FILES})

# Define the include directory for the library
target_include_directories(render PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(chip8 PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(render PUBLIC OpenGL GLU glut)