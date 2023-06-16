add_executable(emulator main.cpp)

target_link_libraries(emulator PUBLIC chip8 render)