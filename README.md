# Music Visualizer

This project aims to construct a visualizer for music. To set up `raylib` in your environment, follow the [steps on GitHub](https://github.com/raysan5/raylib).

### Building `raylib` on Linux

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

Clone the `raylib` repository from GitHub, then compile it:

```bash
git clone https://github.com/raysan5/raylib.git raylib
cd raylib
mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make
sudo make install
```

**Warning**: If you want to compile a different type of library (static, etc.), you must run `make clean` before recompiling.

Insatll GLFW library
```bash
sudo apt-get install libglfw3-dev
```
### Build and Run the Project

```bash
./build.sh
export LD_LIBRARY_PATH=./build/
./build/musializer
```

- Press <key>Q</key> to quit the program
- Press <key>R</key> to hotreload if you make changes in plug in code during execution of musializer

## References

- [Raylib](https://www.raylib.com/): Easy-to-use library for video games.
- [Fourier Transform](https://github.com/realsanjeev/Music-gerne-classification-using-deep-learning/wiki/Fourier-Transform)
- [FFT Implementation](https://rosettacode.org/wiki/Fast_Fourier_transform#Python)
- [X-macros](https://en.wikipedia.org/wiki/X_macro)
- [C Preprocessor](https://en.wikipedia.org/wiki/C_preprocessor)
- [Cdecl: C gibberish to English](https://cdecl.org/?q=float+%28*fs%29%5B2%5D)
