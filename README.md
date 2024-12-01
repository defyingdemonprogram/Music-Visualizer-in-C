# Music Visualizer

<p align=center>
  <img src="./resources/logo/logo-256.png">
</p>

This project aims to construct a visualizer for music and render high quality videos of them. 

## Demo 
https://github.com/user-attachments/assets/008f3f4c-4057-4e10-9ce0-e59c2093aeb3

## Build from Source

### External Dependencies:
- The [ffmpeg](https://ffmpeg.org/) executable must be available in your `PATH` environment variable, as it is invoked as a child process during the build process.

This project utilizes a custom build system written in C called `nob`, which is inspired by [nobuild](https://github.com/tsoding/nobuild). The build system logic is contained in [nob.h](./nob.h), while the main program responsible for triggering the build is [nob.c](./nob.c).

Before using `nob`, you need to bootstrap it by compiling it with a C compiler. On Linux, you can do this with `$ cc -o nob nob.c`, and on Windows (using MSVC and `vcvarsall.bat`), use `$ cl.exe nob.c`. This bootstrap step only needs to be done once. After that, you can use the same executable repeatedly. If you modify [nob.c](./nob.c), the system will attempt to rebuild itself, though this may fail occasionally, requiring a re-bootstrap.

For more details on how the build system works, refer to [nob.c](./nob.c) and [nob.h](./nob.h).

### LINUX
```bash
cc -o nob nob.c  # Run this only once
./nob config -t linux
./nob build
./build/musializer
```

To configure the build settings, run:

```bash
./nob config -t linux -r
```

- The `-t` flag specifies the target platform for compilation. Available options include:
  - `linux`
  - `win64-mingw`
  - `win64-msvc`

- The `-r` flag enables hot reloading during the Musializer app's build process.

Configuration settings are stored in the `./build/build.conf` file, which you can edit directly. For help with configuration, use `./nob config -h`.

Ensure that the `./resources/` directory is present in the same folder as the application when running it.

### Windows (MSVC)
From within `vcvarsall.bat`, execute the following commands:

```cmd
> cl.exe nob.c  # Run this only once
> nob.exe
> build\musializer
```

### Cross Compilation from Linux to Windows using MinGW-w64
First, install [MinGW-w64](https://www.mingw-w64.org/) from your Linux distribution’s repository. Then, execute the following commands:

```bash
cc -o nob nob.c  # Run this only once
./nob config -t win64-mingw
./nob
wine ./build/musializer.exe
```

## Hot Reloading
**Currently supported on Linux only**  
To enable hot reloading, follow these steps:

```bash
cc -o nob nob.c
./nob
./build/musializer
```

Keep the application running. To rebuild, run `./nob build`. For hot reloading, focus on the app window and press <kbd>r</kbd> to reload the application.

## Key Navigation in App

- Press <kbd>Q</kbd> or <kbd>ESC</kbd> to exit the program.
- Press <kbd>R</kbd> to hot reload any changes made to the plugin code while Musializer is running.
- Press <kbd>SPACE</kbd> to toggle pause/play for the music.
- Press <kbd>W</kbd> to restart the music.
- Press <kbd>F</kbd> to save the visualization as an `output.mp4` file.
- Press <kbd>M</kbd> to visualize microphone input, and press <kbd>M</kbd> again to return to the preview UI (available only when the app is ready for you to Drag & Drop the file).

## References

- [Raylib](https://www.raylib.com/): Easy-to-use library for video games.
- [Fourier Transform](https://github.com/realsanjeev/Music-gerne-classification-using-deep-learning/wiki/Fourier-Transform)
- [FFT Implementation](https://rosettacode.org/wiki/Fast_Fourier_transform#Python)
- [X-macros](https://en.wikipedia.org/wiki/X_macro)
- [C Preprocessor](https://en.wikipedia.org/wiki/C_preprocessor)
- [Cdecl: C gibberish to English](https://cdecl.org/?q=float+%28*fs%29%5B2%5D)
- [Music used in demo - credit to penguinmusic](https://pixabay.com/music/future-bass-powerful-beat-121791/)
- [rpath - Wikipedia](https://en.wikipedia.org/wiki/Rpath)
