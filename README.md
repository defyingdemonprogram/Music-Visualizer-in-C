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

This project utilizes a custom build system written in C called `nob`, which is inspired by [nobuild](https://github.com/tsoding/nobuild). The build system logic is contained in [nob.h](./src/nob.h), while the main program responsible for triggering the build is [nob.c](./nob.c).

Before using `nob`, you need to bootstrap it by compiling it with a C compiler. On Linux, you can do this with `$ cc -o nob nob.c`, and on Windows (using MSVC and `vcvarsall.bat`), use `$ cl.exe nob.c`. This bootstrap step only needs to be done once. After that, you can use the same executable repeatedly. If you modify [nob.c](./nob.c), the system will attempt to rebuild itself, though this may fail occasionally, requiring a re-bootstrap.

For more details on how the build system works, refer to [nob.c](./nob.c) and [nob.h](./nob.h).

### LINUX
```bash
cc -o nob nob.c  # Run this only once
./nob
./build/musializer
```
**To compile for your platform, edit the `./build/config.h` file and set the `MUSIALIZER_TARGET` to your platform. Then, run `./nob` to start the compilation process. You can also enable hot reloading by setting `MUSIALIZER_HOTRELOAD`. To enable the microphone in Musializer, set `MUSIALIZER_MICROPHONE`.**

Ensure that the `./resources/` directory is present in the same folder as the application when running it.

If the build fails because of missing header files, you may need to install the X11 dev packages.

On Debian, Ubuntu, etc, do this:
```bash
sudo apt install libx11-dev libxcursor-dev install libxrandr-dev
sudo apt install libxinerama-dev libxi-dev
```

On other distro's, use the appropriate package manager.

### Windows (MSVC)
From within `vcvarsall.bat`, execute the following commands:

```cmd
> cl.exe nob.c  # Run this only once
> nob.exe
> build\musializer
```

### Cross Compilation from Linux to Windows using MinGW-w64
First, install [MinGW-w64](https://www.mingw-w64.org/) from your Linux distribution’s repository. Then, execute the following commands:

**Edit `./build/config.h` and set `MUSIALIZER_TARGET` to `TARGET_WIN64_MINGW`.**
```
./nob
wine ./build/musializer.exe
```

## Hot Reloading
**Currently supported on Linux only**  
To enable hot reloading, follow these steps:
**Edit `./build/config.h` and enable `MUSIALIZER_HOTRELOAD`.**
```
./nob
./build/musializer
```

Keep the application running. To rebuild, run `./nob`. For hot reloading, focus on the app window and press <kbd>h</kbd> to reload the application.

The application works by placing most of its logic into a `libplug` dynamic library, which is reloaded upon request. The [rpath](https://en.wikipedia.org/wiki/Rpath) (also known as the hard-coded run-time search path) for this library is set to `.` and `./build/`. For more details on the configuration, refer to [src/nob_linux.c](src/nob_linux.c).

### Supported Audio Formats
- wav
- ogg
- mp3
- qoa
- xm
- mod

## Key Navigation in App

- Press <kbd>Q</kbd> or <kbd>ESC</kbd> to exit the program.
- Press <kbd>H</kbd> to hot reload any changes made to the plugin code while Musializer is running.
- Press <kbd>F</kdb> to toogle between full screen.
- Press <kbd>M</kdb> to toogle between mute and unmute the music.
- Press <kbd>SPACE</kbd> to toggle pause/play for the music.
- Press <kbd>W</kbd> to restart the music.
- Press <kbd>R</kbd> to save the visualization as an `output.mp4` file.
- Press <kbd>C</kbd> to visualize microphone input, and press <kbd>M</kbd> again to return to the preview UI (available only when the app is ready for you to Drag & Drop the file).

## References

- [Raylib](https://www.raylib.com/): Easy-to-use library for video games.
- [Fourier Transform](https://github.com/realsanjeev/Music-gerne-classification-using-deep-learning/wiki/Fourier-Transform)
- [FFT Implementation](https://rosettacode.org/wiki/Fast_Fourier_transform#Python)
- [X-macros](https://en.wikipedia.org/wiki/X_macro)
- [C Preprocessor](https://en.wikipedia.org/wiki/C_preprocessor)
- [Cdecl: C gibberish to English](https://cdecl.org/?q=float+%28*fs%29%5B2%5D)
- [Music used in demo - credit to penguinmusic](https://pixabay.com/music/future-bass-powerful-beat-121791/)
- [rpath - Wikipedia](https://en.wikipedia.org/wiki/Rpath)
