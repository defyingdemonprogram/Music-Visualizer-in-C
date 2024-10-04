#!/bin/sh

set -xe

CFLAGS="-Wall -Wextra -ggdb `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib` `pkg-config --libs glfw3` -lm -ldl -lpthread"

mkdir -p ./build/
if [ ! -z "${HOTRELOAD}" ]; then
    clang $CFLAGS -o ./build/libplug.so -fPIC -shared ./src/plug.c ./src/ffmpeg_linux.c $LIBS
    # for hotreloading
    clang $CFLAGS -DHOTRELOAD -o ./build/musializer ./src/main.c $LIBS
else
    clang $CFLAGS -o ./build/musializer ./src/plug.c ./src/ffmpeg_linux.c ./src/main.c $LIBS
fi
