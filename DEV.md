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

### Explanation of `build.sh`

```bash
#!/bin/sh

set -xe

CFLAGS="-Wall -Wextra -ggdb `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib` `pkg-config --libs glfw3` -lm -ldl -lpthread"

mkdir -p ./build/
if [ ! -z "${HOTRELOAD}" ]; then
    clang $CFLAGS -o ./build/libplug.so -fPIC -shared ./src/plug.c ./src/ffmpeg_linux.c $LIBS
    # for hotreloading
    clang $CFLAGS -DHOTRELOAD -o ./build/musializer ./src/main.c $LIBS -L./build/ -Wl,-rpath=./build/ -Wl,-rpath=./
else
    clang $CFLAGS -o ./build/musializer ./src/plug.c ./src/ffmpeg_linux.c ./src/main.c $LIBS -L./build/
fi

cp -r ./resources/ ./build/
```
- **`set -xe`**: This option combination causes the script to print each command before executing it (`-x`) and to exit immediately if any command fails (`-e`).

- **`CFLAGS="-Wall -Wextra $(pkg-config --cflags raylib)"`**: Sets the compiler flags:
    - `-Wall`: Enables most common warning messages, helping to catch potential bugs.
    - `-Wextra`: Enables additional warning messages not covered by `-Wall`, providing more thorough checks.
    - `$(pkg-config --cflags raylib)`: Retrieves necessary compiler flags for the `raylib` library.

- **`LIBS="$(pkg-config --libs raylib) $(pkg-config --libs glfw3) -lm -ldl -lpthread"`**: Specifies the linker flags:
    - `$(pkg-config --libs raylib)`: Retrieves linker flags required to link against `raylib`.
    - `$(pkg-config --libs glfw3)`: Retrieves linker flags required to link against GLFW.
    - `-lm`: Links against the math library.
    - `-ldl`: Links against the dynamic linking library.
    - `-lpthread`: Links against the POSIX threads library.

- **`mkdir -p ./build/`**: Creates the `build` directory if it doesn't already exist.

- **Conditional Compilation**:
  - If `HOTRELOAD` is set:
    - `clang $CFLAGS -o ./build/libplug.so -fPIC -shared ./src/plug.c ./src/ffmpeg_linux.c $LIBS`: Compiles the shared library for hot reloading.
    - `clang $CFLAGS -DHOTRELOAD -o ./build/musializer ./src/main.c $LIBS -L./build/ -Wl,-rpath=./build/ -Wl,-rpath=./`: 
      - `-L./build/`: Adds the `./build` directory to the library search path, allowing the linker to find the shared libraries located there.
      - `-Wl,-rpath=./build/`: Sets the runtime library search path to `./build`, so that when the executable is run, it knows where to look for shared libraries.
      - `-Wl,-rpath=./`: Additionally sets the runtime library search path to the current directory. This can help ensure that if any libraries are also present there, they can be found without needing to specify their full path.

  - If `HOTRELOAD` is not set:
    - `clang $CFLAGS -o ./build/musializer ./src/plug.c ./src/ffmpeg_linux.c ./src/main.c $LIBS -L./build/`: Compiles everything into the main executable without the hot reloading support.

- **`cp -r ./resources/ ./build/`**: Copies the resources directory into the build directory. 


### Building `raylib` on Linux
To set up `raylib` in your environment, follow the [steps on GitHub](https://github.com/raysan5/raylib).

You need **GCC** (or an alternative C99 compiler), **make**, **git**, and **CMake** to build the system:

```bash
sudo apt install build-essential git
sudo apt install cmake
```

Required libraries include **ALSA** for audio, **Mesa** for OpenGL accelerated graphics, and **X11** for windowing:

```bash
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```

#### Building `raylib` using `make`

You can compile three different types of `raylib` libraries:
- Static library (default method)
- Dynamic shared library (often used on Linux)
- Web library

**Clone the `raylib` repository from GitHub, then compile it as dynamic shared library:**

```bash
git clone https://github.com/raysan5/raylib.git raylib
cd raylib
mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make
sudo make install
```

**Warning**: If you want to compile a different type of library (static, etc.), you must run `make clean` before recompiling.

**Insatll GLFW library**
```bash
sudo apt-get install libglfw3-dev
```
### Build and Run the Project

For **Hot Reloading**, execute the following commands:

```bash
export HOTRELOAD=1
./build_posix_clang.sh
export LD_LIBRARY_PATH=./build/
./build/musializer
```

This process works by encapsulating the majority of the application logic within a `libplug` dynamic library, which can be reloaded on demand. The [rpath](https://en.wikipedia.org/wiki/Rpath) (i.e., the hard-coded run-time search path) for this library is set to `.` and `./build/`. For additional details on the configuration, please refer to above explanation of `build.sh`.


### Converting music format with other using `ffmpeg`
To convert one file extenstion to other, (note only changing the extention doesnot actually change the encodeing of encoding of that format.)
```bash
ffmpeg -i original_audio destination_audio.mp3
```
Learn more about ffmpeg in [official documentation - ffmpeg documentation](https://ffmpeg.org/documentation.html)

### Some Commands

- **`memcpy`**: This function copies a block of memory from one location to another. It requires specifying the number of bytes to copy, which must not exceed the size of the source memory block.
- **`memset`**: This function sets a block of memory to a specific value.
- **`memmove`**: This function moves a block of memory from one location to another, handling overlapping regions correctly.

```c
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
```

### Example Usage

```c
memcpy(dest, src, n);
memset(buffer, value, n);
memmove(dest, src, n);
```

### `gcc` Compilation Command with Library Linking

To compile a C program and link it with the math library, use:

```bash
gcc -o fft fft.c -lm
```
- **`-lm`**: This flag tells the linker to include the math library (`libm`), which provides mathematical functions such as `sin()`, `cos()`, and `sqrt()`, among others, defined in `math.h`.

For additional warnings and using `clang`:

```bash
clang -Wall -Wextra -o fft fft.c -lm
```

To check the shared libraries that `libplug.so` depends on:

```bash
ldd libplug.so
```

### C Preprocessor (CPP)

The C Preprocessor (CPP) processes C source code before compilation. It handles tasks such as macro expansion, file inclusion, and conditional compilation.

To preprocess a C source file:

```bash
cpp foo.c
```

### Hot Reloading

Hot reloading allows you to apply code changes without restarting the application. It enables you to modify code, rebuild the executable, and see changes without restarting the application.

To set up hot reloading for `music-visualizer`, use dynamically linked libraries and build `raylib` as a shared object:

1. **Configure `raylib` for Shared Libraries**:

   In the `raylib/build/CMakeCache.txt` file, set the `BUILD_SHARED_LIBS` option to `ON`:

   ```cmake
   BUILD_SHARED_LIBS:BOOL=ON
   ```

2. **Build `raylib`**:

   Run the following command to build `raylib` with shared libraries:

   ```bash
   make -j3
   ```

   This command uses 3 parallel jobs to speed up the build process.

By following these steps, you can enable hot reloading for your application, facilitating faster development and easier testing of code changes.

### Difference Between Two `typedef` Declarations

| Aspect                               | `typedef void(*plug_hello_t)(void);`                  | `typedef void(plug_hello_t)(void);`                     |
|--------------------------------------|--------------------------------------------------------|----------------------------------------------------------|
| **Definition**                       | Creates an alias for a pointer to a function.         | Creates an alias for a function type itself.            |
| **Type Aliased**                     | Pointer to a function: `void(*)(void)`                | Function type: `void(void)`                             |
| **Usage**                            | Defines a pointer type used to store function pointers. | Defines a function type used to declare functions directly. |
| **Declaration of Function Pointer**  | `plug_hello_t ptr = my_function;`                      | Not applicable; declare functions directly with `void my_function(void);` |
| **Declaration of Function**          | `void my_function(void);`                              | `plug_hello_t another_function = my_function;` (less common) |
| **Example Code**                     | ```c                                                    | ```c                                                     |
|                                      | typedef void(*plug_hello_t)(void);                     | typedef void(plug_hello_t)(void);                        |
|                                      | void my_function(void) { /*...*/ }                     | void my_function(void) { /*...*/ }                       |
|                                      | plug_hello_t ptr = my_function; // `ptr` is a pointer to a function | plug_hello_t another_function = my_function; // direct function declaration |
|                                      | ```                                                    | ```                                                     |
| **Common Use Case**                  | Used for callbacks and function pointers.              | Less common; generally used for direct function declarations. |

### Changing How the Program Initializes the State
From `features/drag-and-drop` branch

Previously, the application was responsible for allocating the state and passing it to the plugin during each frame. This setup required methods like `plug_update`. Moving forward, the responsibility for allocating the state will shift from the application to the plugin itself.

![Changing the Allocation of State from `main.c` to Plugin](images/change_state_diagram.png)

### Removing the Artifacts in rendered Graphics
From `4-feature/shaders` branch

**Aliasing** refers to the visual artifacts that occur when a high-frequency signal is sampled at a rate that is insufficient to accurately represent it. In digital images, this can manifest as jagged edges or "stair-stepping" on diagonal lines and curves, making the image appear less smooth or pixelated.

**Anti-Aliasing** is a technique used to reduce the effects of aliasing, creating smoother transitions and reducing jagged edges in images. Here are some common types of anti-aliasing:

1. **Spatial Anti-Aliasing**:
   - **Supersampling**: Renders the image at a higher resolution and then downsamples it to the desired resolution. This averages out the color of pixels, resulting in smoother edges.
   - **Multisampling**: Samples multiple points within each pixel and averages the results. It's more efficient than supersampling since it only processes the edges where needed. In Raylib, we enable flag for MSAA(Multisample Anti-Aliasing (MSAA)) with a 4x sampling rate by `SetConfigFlags(FLAG_MSAA_4X_HINT);` as 
   - More on: [Evaluating Different Spatial Anti Aliasing Techniques - PDF](https://kth.diva-portal.org/smash/get/diva2:1106244/FULLTEXT01.pdf)

2. **Post-Processing Anti-Aliasing**:
   - **FXAA (Fast Approximate Anti-Aliasing)**: A screen-space anti-aliasing technique that smooths edges based on the contrast of pixels, resulting in a softer image without heavy computational costs.
   - **SMAA (Subpixel Morphological Anti-Aliasing)**: Combines techniques from both morphological and edge detection methods, providing high-quality results with less performance impact than supersampling.
   - More details on [SMAA vs FXAA: A Guide to Anti-Aliasing Techniques](https://9meters.com/technology/graphics/smaa-vs-fxaa-a-guide-to-anti-aliasing-techniques)

3. **Temporal Anti-Aliasing**:
   - **TAA (Temporal Anti-Aliasing)**: Utilizes information from previous frames to smooth out edges over time, helping to reduce flickering and aliasing without significantly impacting performance.
   - More details on [Temporal Anti-Aliasing (TAA) - developer.unigine.com](https://developer.unigine.com/en/docs/latest/principles/render/antialiasing/taa)

### RPATH

rpath (runtime library search path) is a mechanism used in computing to specify the locations where a program should look for shared libraries at runtime. It is particularly relevant in the context of dynamic linking, where executables or shared libraries need to locate their required dependencies. The advantanges of using rpath are:
- **Ease of Deployment**: By embedding library paths directly into executables, developers can create relocatable applications that do not require users to set up library paths manually.
- **Version Control**: Rpath allows applications to use specific versions of libraries without conflicts with system-wide installations, facilitating easier management of dependencies.

### Window Support for Musializer

**Branch**: `8-enhancement/window-support`

Since we are using Linux to develop a Windows application, we can leverage several methods and tools that facilitate cross-platform development. The main tools used in our development are:

#### Development Tools

1. **MinGW (Minimalist GNU for Windows)**  
   - Provides a GCC (GNU Compiler Collection) that targets Windows. 
   - Allows the inclusion of Windows-specific headers like `windows.h` directly in our code.  
   - **Installation**:
     - Download from the [official website](https://www.mingw-w64.org/downloads/#ubuntu).
     - Alternatively, install using `apt` following the instructions in this [GitHub issue](https://github.com/ggerganov/whisper.cpp/issues/168).

2. **Wine**  
   - Enables the running of Windows applications on Linux. 
   - After compiling the application with MinGW, Wine can be used to test the executable directly on a Linux machine.  
   - **Installation**: Follow the instructions to [download and install Wine based on your OS](https://gitlab.winehq.org/wine/wine/-/wikis/Download).

### Additional Dependencies

- **raylib**: Download the `raylib` library (win64-mingw-w64 version) from the [GitHub releases page](https://github.com/raysan5/raylib/releases/tag/5.0). Look for the file named `raylib-5.0_win64_mingw-w64.zip`.

- **ffmpeg**: Download the Windows binaries from [gyan dev](https://www.gyan.dev/ffmpeg/builds/). After unzipping the downloaded file, place `ffmpeg.exe` in the same directory as your project. Alternatively, you can add ffmpeg to the system environment path (you can view the current paths by running `path` in the Command Prompt). If you choose to add it to the environment path, update the command in `ffmpeg_windows.c` to replace `ffmpeg.exe` with `ffmpeg`.

#### Script for Compiling the Windows Application

```bash
# Compile the program
x86_64-w64-mingw32-gcc -mwindows -Wall -Wextra -ggdb \
    -Iraylib-5.0_win64_mingw-w64/include/ \
    -o ./build/musializer.exe \
    ./src/plug.c ./src/ffmpeg_windows.c ./src/main.c \
    -Lraylib-5.0_win64_mingw-w64/lib/ \
    -lraylib -lwinmm -lgdi32 -static
```

#### Explanation of the Script

- **Compiler Invocation**: `x86_64-w64-mingw32-gcc` is compiler targeting Windows from a Linux environment.
  
- **Flags**:
  - `-mwindows`: Indicates a Windows GUI application.
  - `-Wall`: Enables all compiler warning messages.
  - `-Wextra`: Enables additional warning messages.
  - `-ggdb`: Includes debugging information for GDB.

- **Include Directory**: `-Iraylib-5.0_win64_mingw-w64/include/` specify path to the header files for the Raylib library.
  
- **Output File**: `-o ./build/musializer.exe` specifies the name and location of the output executable.
  
- **Source Files**: `./src/plug.c`, `./src/ffmpeg_windows.c` and `./src/main.c`
  
- **Library Directory**: `-Lraylib-5.0_win64_mingw-w64/lib/` specify path to the library files.

- **Linked Libraries**: 
  - `-lraylib`: Links the Raylib library.
  - `-lwinmm`: Links the Windows multimedia library.
  - `-lgdi32`: Links the Windows GDI (Graphics Device Interface) library.

- **Static Linking**: `-static` ensures that libraries are linked statically, so the executable does not depend on dynamic libraries at runtime.

**Running the Compiled Application**

After compilation with `mingw-gcc`, the executable can be found at `./build/musializer.exe`. To run the `.exe` file in the Linux environment, use the following command:

```bash
wine ./build/musializer.exe
```


### Added Feature to support the user audio recording and play
**Branch**: `8-features/microphone`

In this branch, we utilize the `miniaudio` library to record audio from the user's microphone and visualize that sound. Care must be taken when integrating `miniaudio` with `raylib`, as symbol collisions may occur, leading to compilation issues.

#### Symbol Collision

Including `miniaudio.h` directly alongside `raylib.h` can result in conflicts due to overlapping symbol definitions, especially when both libraries are included in the same translation unit. If you do not encounter errors during compilation, it may be due to dynamic linking of `raylib`, which can obscure symbol conflicts that would typically arise in static linking scenarios.

#### Windows-Specific Issues

For Windows users, a common problem is the interaction between `miniaudio.h` and `windows.h`. The overlapping symbols from these headers can lead to undefined references or compilation errors. To address this, we created a separate translation unit with the header file `separate_translation_unit_for_minimal_audio.h`. This file allows us to define an interface for initializing and managing audio devices without directly including `miniaudio.h`, thereby avoiding potential collisions with `windows.h` and `raylib.h`.

#### Resolving Symbol Collisions

1. **Separate Translation Unit**: We utilize the `separate_translation_unit_for_minimal_audio.h` header to encapsulate audio functionality, reducing the likelihood of symbol collisions with other libraries.

2. **Callback Mechanism**: A callback function type (`user_callback2_t`) has been implemented, enabling user-defined audio processing without a direct dependency on `miniaudio.h`.

3. **Dynamic vs. Static Linking**: Consider your linking strategy carefully. While dynamic linking can help minimize some collisions, thorough testing across various platforms is essential.

**Additional Resources**: For more in-depth information on using `miniaudio`, please refer to the [official documentation](https://miniaud.io/docs/manual/index.html) and the [official GitHub repository](https://github.com/mackron/miniaudio). These resources offer comprehensive guidance on library integration and common troubleshooting.

### Build System Using C  
**Branch**: `10-enhancement/build-with-c`

In this branch, we implement a build system using **C** instead of relying on a shell script for the program.

We primarily use `cc` (the C compiler) rather than `clang` to build **raylib** and **musializer**. The issue arises when using `clang` in the build process (specifically in `nob.c`), as an error is encountered in `./raylib/raylib-5.0/src/external/glfw/src/posix_poll.c` at line 49:

```
error: call to undeclared function 'ppoll'
```

This error was resolved by switching from `clang` to `cc`.  
**Note**: The exact cause of the error is unclear, as changing the compiler affects the entire build process.

### Difference Between `clang` and `cc`

| **Aspect**             | **`clang`**                             | **`cc`**                                    |
|------------------------|-----------------------------------------|---------------------------------------------|
| **Type**               | Specific compiler (LLVM)                | Generic name for a C compiler               |
| **Platform**           | Commonly used on macOS, but also available on Linux and other platforms | Available on Unix-like systems (e.g., Linux, macOS) |
| **Default Compiler**   | Default on macOS, can be used independently on Linux | A symbolic link to the system’s default C compiler (e.g., `clang` or `gcc`) |
| **Usage**              | Used explicitly for LLVM toolchain and features | A more generic command for invoking the system's default compiler |
| **Error Messages**     | Known for detailed and user-friendly diagnostics | Error messages depend on the system's default compiler (either `clang` or `gcc`) |
| **Performance**        | Generally faster with better optimizations, especially on macOS | Depends on the compiler it points to (e.g., `gcc` or `clang`) |
| **Language Support**   | Supports C, C++, Objective-C, and other languages via LLVM | Primarily for C (but could invoke other compilers that support C++) |
| **Main Focus**         | Optimized for modern C/C++ development, debugging, and diagnostics | Used for general-purpose C compilation |


#### Comparison of **MinGW** and **MSVC GCC** compilers

| **Aspect**                | **MinGW (GCC)**                                    | **MSVC (Microsoft Visual C++)**                     |
|---------------------------|----------------------------------------------------|-----------------------------------------------------|
| **Compiler**              | GNU Compiler Collection (GCC)                     | Microsoft Visual C++ Compiler                       |
| **Platform**              | Cross-platform, but focused on Windows for MinGW   | Windows-only                                        |
| **Toolchain**             | GNU Binutils, GCC, and GNU Standard Library (libc) | Microsoft’s proprietary toolchain (MSVC CRT)        |
| **Target**                | Produces Windows executables with a focus on compatibility with the Windows API | Primarily Windows executables optimized for Windows environments |
| **Standard Library**      | GNU C Library (glibc) or Newlib                    | Microsoft C Runtime Library (CRT)                   |
| **ABI (Application Binary Interface)** | Follows the **Microsoft ABI** (Binary Interface), but with GCC-style calling conventions | Uses its own **Microsoft ABI**, which is Windows-specific |
| **Compatibility**         | Compatible with Windows API, but may lack full integration with Windows-specific libraries like MFC or DirectX | Fully compatible with Windows libraries, COM, MFC, and other Windows-specific frameworks |
| **Code Generation**       | Typically produces more portable code for cross-platform development | Optimized for Windows performance and integration |
| **Platform Integration**  | Minimal integration with Windows-specific APIs; better for UNIX-like environments | Deep integration with Windows OS, APIs, and technologies |
| **Linking**               | Uses GNU linker (ld)                              | Uses Microsoft's linker (link.exe)                  |
| **Windows Features**      | Limited support for advanced Windows features (like DirectX, MFC) | Full support for Windows-specific technologies, including MFC, COM, and DirectX |
| **Debugging Support**     | Supports debugging through GDB (GNU Debugger)     | Deep debugging support through Visual Studio Debugger |
| **IDE Integration**       | Typically used with lightweight editors (e.g., VS Code, Sublime Text) or through command line | Full integration with Microsoft Visual Studio IDE |
| **Optimizations**         | Focuses on general performance and cross-platform optimization | Advanced Windows-specific optimizations (e.g., for threading, performance on Windows) |

### References:
- [FFMPEG official documentation - ffmpeg documentation](https://ffmpeg.org/documentation.html)
- [Anti-aliasing - wikipedia](https://en.wikipedia.org/wiki/Anti-aliasing)
- [Anti-Aliasing Detailed Explanation- OpenGL](https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing)
- [Shader Programming - Blogpost](https://clauswilke.com/art/post/shaders)
- [Learn all about shaders](https://iquilezles.org/articles/)
- [Understanding RPATH - duerrenberger.dev](https://duerrenberger.dev/blog/2021/08/04/understanding-rpath-with-cmake/)
- [MinGW-w64 official website](https://www.mingw-w64.org/)
- [Wine -- Official Website](https://www.winehq.org/)
- [MINIAUDIO Official Documentation](https://miniaud.io/docs/manual/index.html)
- [MINIAUDIO Official GitHub](https://github.com/mackron/miniaudio)
- [MinGW vs MSVC -- wiki](https://wiki.strawberrymusicplayer.org/wiki/MinGW_vs_MSVC)