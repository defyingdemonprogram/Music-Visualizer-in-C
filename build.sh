#!/bin/sh

set -xe

CFLAGS="-Wall -Wextra -ggdb `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib` -lglfw -lm -ldl -lpthread"

mkdir -p ./build/
clang $CFLAGS -o ./build/libplug.so -fPIC -shared ./src/plug.c $LIBS
# for hotreloading
clang $CFLAGS -DHOTRELOAD -o ./build/musializer ./src/main.c $LIBS
# clang $CFLAGS -o ./build/musializer ./src/plug.c ./src/main.c $LIBS
clang -o ./build/fft ./src/fft.c -lm
