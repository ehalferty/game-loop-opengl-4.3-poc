#!/bin/bash

rm -rf "app.exe"

./build_game_java.sh

#cp Game.class ./cmake*/

#javac Game.java
# This header isn't actually used, I just regenerate it for reference.
#javah Game

x86_64-w64-mingw32-g++ \
stb_image.h main.h main.cpp \
-std=c++11 -lgdi32 -lgdiplus -lopengl32 -Wl,--subsystem,windows \
-o "app.exe" \
&& ./*exe*

# gcc -D UNICODE -D _UNICODE -c myfile.c -o myfile.o
# windres -i resource.rc -o resource.o
# gcc -o myapplication.exe myfile.o resource.o -s -lcomctl32 -Wl,--subsystem,windows
