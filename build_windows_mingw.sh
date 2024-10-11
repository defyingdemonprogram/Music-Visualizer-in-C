#!/bin/sh

set -xe

mkdir -p ./build/

# Compile the program
x86_64-w64-mingw32-gcc -mwindows -Wall -Wextra -ggdb \
    -Iraylib-4.5.0_win64_mingw-w64/include/ \
    -o ./build/musializer.exe \
    ./src/plug.c ./src/ffmpeg_windows.c ./src/musializer.c \
    -Lraylib-4.5.0_win64_mingw-w64/lib/ \
    -lraylib -lwinmm -lgdi32 -static

# Copy resources to the build directory
cp -r ./resources ./build
