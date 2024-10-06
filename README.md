# Music Visualizer

This project aims to construct a visualizer for music. 
## Demo 
https://github.com/user-attachments/assets/008f3f4c-4057-4e10-9ce0-e59c2093aeb3

To set up `raylib` in your environment, follow the [steps on GitHub](https://github.com/raysan5/raylib).

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

```bash
./build.sh
export LD_LIBRARY_PATH=./build/
./build/musializer
```

- Press <kbd>Q</kbd> to exit the program.
- Press <kbd>R</kbd> to hot reload any changes made to the plugin code while Musializer is running.
- Press <kbd>SPACE</kbd> to toggle pause/play for the music.
- Press <kbd>W</kbd> to restart the music.
- Press <kbd>F</kbd> to save the visualization as an `output.mp4` file.

**Note**: Once the music visualization completes the `ffmpeg` rendering, it doesnot return to playing music. Press <kbd>W</kbd> to start the music in the app.

## References

- [Raylib](https://www.raylib.com/): Easy-to-use library for video games.
- [Fourier Transform](https://github.com/realsanjeev/Music-gerne-classification-using-deep-learning/wiki/Fourier-Transform)
- [FFT Implementation](https://rosettacode.org/wiki/Fast_Fourier_transform#Python)
- [X-macros](https://en.wikipedia.org/wiki/X_macro)
- [C Preprocessor](https://en.wikipedia.org/wiki/C_preprocessor)
- [Cdecl: C gibberish to English](https://cdecl.org/?q=float+%28*fs%29%5B2%5D)
- [Music used in demo - credit to penguinmusic](https://pixabay.com/music/future-bass-powerful-beat-121791/)
