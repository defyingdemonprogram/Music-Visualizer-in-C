#!/bin/sh

set -xe

# Define compilation flags and libraries
CFLAGS="-Wall -Wextra -ggdb $(pkg-config --cflags raylib)"
LIBS="$(pkg-config --libs raylib) -lglfw -lm -ldl -lpthread"

# Ensure .env has execute permissions
chmod +x .env

# Execute .env script
./.env

# Create the build directory if it doesn't exist
mkdir -p ./build/

# Compile libplug.so as a position-independent shared library
clang $CFLAGS -o ./build/libplug.so -fPIC -shared ./src/plug.c $LIBS

# Compile musializer and link it against libplug.so in the build directory
clang $CFLAGS -o ./build/musializer ./src/main.c $LIBS -L./build -lplug

# Compile fft with the math library
clang -o ./build/fft ./src/fft.c -lm
