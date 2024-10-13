# Music Visualizer

<p align=center>
  <img src="./logo/logo-256.png">
</p>

This project aims to construct a visualizer for music and render high quality videos of them. 

## Demo 
https://github.com/user-attachments/assets/008f3f4c-4057-4e10-9ce0-e59c2093aeb3

## Quick Start

Dependencies: 
- [raylib](https://www.raylib.com/) and all its associated transitive dependencies.
- [ffmpeg](https://ffmpeg.org/) executable available in `PATH` environment variable. (it is called as a child process)


The project provides a bunch of build shell scripts that have the following naming scheme `build_<platform>_<compiler>.sh`. Pick the appropriate one.

### POSIX

```console
./build_posix_clang.sh
./build/musializer
```

Keep in mind that the application needs [./resources/](./resources/) to be present in the folder it is ran from.

### Windows

Windows support is at very early stage right now. Since I don't have a convenient Windows Development Environment, I'm cross compiling Musializer with [MinGW](https://www.mingw-w64.org/). See [./build_windows_mingw.sh](./build_windows_mingw.sh) for more information.


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

This process works by encapsulating the majority of the application logic within a `libplug` dynamic library, which can be reloaded on demand. The [rpath](https://en.wikipedia.org/wiki/Rpath) (i.e., the hard-coded run-time search path) for this library is set to `.` and `./build/`. For additional details on the configuration, please refer to [build.sh](./build.sh).

- Press <kbd>Q</kbd> or <kbd>ESC</kbd> to exit the program.
- Press <kbd>R</kbd> to hot reload any changes made to the plugin code while Musializer is running.
- Press <kbd>SPACE</kbd> to toggle pause/play for the music.
- Press <kbd>W</kbd> to restart the music.
- Press <kbd>F</kbd> to save the visualization as an `output.mp4` file.

## References

- [Raylib](https://www.raylib.com/): Easy-to-use library for video games.
- [Fourier Transform](https://github.com/realsanjeev/Music-gerne-classification-using-deep-learning/wiki/Fourier-Transform)
- [FFT Implementation](https://rosettacode.org/wiki/Fast_Fourier_transform#Python)
- [X-macros](https://en.wikipedia.org/wiki/X_macro)
- [C Preprocessor](https://en.wikipedia.org/wiki/C_preprocessor)
- [Cdecl: C gibberish to English](https://cdecl.org/?q=float+%28*fs%29%5B2%5D)
- [Music used in demo - credit to penguinmusic](https://pixabay.com/music/future-bass-powerful-beat-121791/)
- [rpath - Wikipedia](https://en.wikipedia.org/wiki/Rpath)
