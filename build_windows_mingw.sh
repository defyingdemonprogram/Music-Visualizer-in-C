#!/bin/sh

set -xe

mkdir -p ./build/

x86_64-w64-mingw32-windres ./logo/musializer.rc -O coff -o ./build/musializer.res
# Compile the program
x86_64-w64-mingw32-gcc -mwindows -Wall -Wextra -ggdb \
    -Iraylib-5.0_win64_mingw-w64/include/ \
    -o ./build/musializer.exe \
    ./src/plug.c ./src/ffmpeg_windows.c ./src/main.c ./build/musializer.res \
    -Lraylib-5.0_win64_mingw-w64/lib/ \
    -lraylib -lwinmm -lgdi32 -static

# Copy resources to the build directory
cp -r ./resources ./build
