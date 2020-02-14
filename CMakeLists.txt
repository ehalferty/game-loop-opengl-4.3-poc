cmake_minimum_required(VERSION 3.9)
project(game_loop_opengl_4_3_poc)

set(CMAKE_CXX_STANDARD 11)
link_libraries( -lgdi32 -lgdiplus -lopengl32 -Wl,--subsystem,windows)
#set(CMAKE_CXX_FLAGS "-std=c++11 -lgdi32 -lopengl32 -Wl,--subsystem,windows")

#target_compile_options(game_loop_opengl_4_3_poc PRIVATE -fexceptions)

add_executable(game_loop_opengl_4_3_poc
        main.cpp main.h)
