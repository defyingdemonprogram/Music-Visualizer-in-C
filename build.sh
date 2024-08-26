#!/usr/bin/sh

set -xe

CFLAGS="-Wall -Wextra `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib`"

chmod +700 ./.env
source .env
clang $CFLAGS -o musializer main.c $LIBS
