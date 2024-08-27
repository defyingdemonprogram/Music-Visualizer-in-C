#!/usr/bin/sh

set -xe

CFLAGS="-Wall -Wextra `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib` -lglfw -lm -ldl -lpthread"

chmod +700 ./.env
./.env
mkdir -p ./build/
clang $CFLAGS -o ./build/musializer ./src/main.c $LIBS
clang -o ./build/fft ./src/fft.c -lm
