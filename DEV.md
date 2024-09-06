## Difference between `clang` and `gcc`

| Feature                    | GCC (GNU Compiler Collection)                               | Clang (LLVM Compiler)                                   |
|----------------------------|-------------------------------------------------------------|----------------------------------------------------------|
| **Origins**                | Developed by Free Software Foundation (FSF) since 1987.    | Part of the LLVM project, started in 2007.              |
| **Supported Languages**    | C, C++, Fortran, Ada, Go, D, and others.                    | C, C++, Objective-C, Objective-C++, and others.         |
| **Code Generation**        | Robust, widely used in production environments.             | Modern, often praised for readable code generation.     |
| **Optimization**           | Extensive optimization support, known for mature optimizations. | Excellent optimization with modern techniques.          |
| **Error Messages**         | Useful but sometimes less user-friendly.                    | Clear, detailed, and user-friendly diagnostics.          |
| **Build System Integration** | Integrates well with many build systems, default in many Linux distros. | Strong support in LLVM ecosystem, works well with CMake. |
| **Licensing**              | GNU General Public License (GPL).                            | LLVM License, more permissive.                           |
| **Tooling**                | Includes tools like `gdb`, `binutils`.                      | Part of LLVM, includes tools like `lld`, `clang-tidy`.  |
| **Performance**            | Known for mature and stable performance.                     | Often provides faster compile times.                    |
| **Compatibility**          | Broad architecture support, well-established.               | Modular architecture, frequent updates.                 |

After building `raylib` as per the README, set the `.env` file as follows:

```bash
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```

## Explanation of `build.sh`

```bash
#!/usr/bin/sh

set -xe

CFLAGS="-Wall -Wextra $(pkg-config --cflags raylib)"
LIBS="$(pkg-config --libs raylib) -lglfw -lm -ldl -lpthread"

chmod +700 ./.env
./.env
mkdir -p ./build/
clang $CFLAGS -o ./build/musializer ./src/main.c $LIBS
clang -o ./build/fft ./src/fft.c -lm
```

- `set -xe`: This option combination causes the script to print each command before executing it (`-x`) and to exit immediately if any command fails (`-e`).
- `clang`: This command is used to compile the source files into executables.
  - `clang $CFLAGS -o ./build/musializer ./src/main.c $LIBS`: Compiles `main.c` with the specified `CFLAGS` and creates the `musializer` executable in the `./build` directory.
  - `clang -o ./build/fft ./src/fft.c -lm`: Compiles `fft.c` into an executable named `fft` in the `./build` directory, linking against the math library (`-lm`).
- `CFLAGS="-Wall -Wextra $(pkg-config --cflags raylib)"`: Sets the compiler flags:
  - `-Wall`: Enables most common warning messages, which helps to catch potential bugs.
  - `-Wextra`: Enables additional warning messages not covered by `-Wall`, providing more thorough checks.
  - `pkg-config --cflags raylib`: Retrieves the necessary compiler flags for the `raylib` library, which are added to `CFLAGS`.
- `LIBS="$(pkg-config --libs raylib) -lglfw -lm -ldl -lpthread"`: Specifies the linker flags:
  - `pkg-config --libs raylib`: Retrieves the linker flags required to link against `raylib`.
  - `-lglfw`: Links against the GLFW library.
  - `-lm`: Links against the math library.
  - `-ldl`: Links against the dynamic linking library.
  - `-lpthread`: Links against the POSIX threads library.

To convert one file extenstion to other, (note only changing the extention doesnot actually change the encodeing of encoding of that format.)
```bash
ffmpeg -i original_audio destination_audio.mp3
```

Here’s a revised version of the content with improved clarity and formatting:

### Some Commands

**`memcpy`**: This function copies memory from one location to another. It is used to specify the range of bytes to copy, which should not exceed the size of the source memory block.

```c
void *memcpy(void *dest, const void* src, size_t n);
```

### `gcc` Compilation Command with Library Linking

To compile a C program with the math library:

```bash
gcc -o fft fft.c -lm
```
- **`-lm`**: This flag tells the linker to include the math library (`libm`). This is required for mathematical functions such as `sin()`, `cos()`, `sqrt()`, etc., which are defined in `math.h`.

Alternatively, using `clang` with additional warnings enabled:

```bash
clang -Wall -Wextra -o fft fft.c -lm
```

To show the shared libraries that `libplug.so` depends on:

```bash
ldd libplug.so
```

## Hot Reloading

Hot reloading allows you to apply code changes without restarting the application. Typically, you modify code, rebuild the executable, and then run the new executable. With hot reloading, you can run the application in the background, and the changes you make to the code will be reflected in the running application without having to restart it.

To enable hot reloading for `music-visualizer`, you need to use dynamically linked libraries and build `raylib` as a shared object.

1. **Configure `raylib` for Shared Library**:

   In the `raylib/build/CMakeCache.txt` file, ensure that the `BUILD_SHARED_LIBS` option is set to `ON`:

   ```cmake
   BUILD_SHARED_LIBS:BOOL=ON
   ```

2. **Build `raylib`**:

   Run the following command to build `raylib` with shared libraries:

   ```bash
   make -j3
   ```

   This command compiles `raylib` using 3 parallel jobs, which speeds up the build process.

By following these steps, you can set up hot reloading for your application, allowing for faster development cycles and easier testing of code changes.
