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

## DFT vs FFT

The Fourier Transform is essential in music analysis for several reasons. It enables the decomposition of complex audio signals into their constituent frequencies, which is crucial for understanding, processing, and analyzing music. 

| **Aspect**            | **DFT (Discrete Fourier Transform)** | **FFT (Fast Fourier Transform)**  |
|-----------------------|--------------------------------------|-----------------------------------|
| **Definition**        | Mathematical transform converting a discrete signal from time/space to frequency domain | Algorithm to compute the DFT efficiently |
| **Complexity**        | \(O(N^2)\)                           | \(O(N \log N)\)                   |
| **Efficiency**        | Less efficient, especially for large \(N\) | Highly efficient, especially for large \(N\) |
| **Calculation**       | Requires \(N^2\) operations (multiplications and additions) | Uses divide-and-conquer techniques to reduce operations |
| **Usage**             | Theoretical contexts or small datasets | Practical applications like signal processing, image analysis |
| **Example Size**      | Suitable for small datasets          | Suitable for large datasets       |
| **Practicality**      | Less practical for large data due to high computation time | Widely used in practice for large data sets due to efficiency |

### Samples and Frames

| **Aspect**            | **Samples**                                               | **Frames**                                                 |
|-----------------------|-----------------------------------------------------------|------------------------------------------------------------|
| **Definition**        | Individual data points representing the amplitude of the audio signal at a specific point in time | Groups of samples processed together as a single unit |
| **Purpose**           | Capturing the waveform of the audio signal                | Facilitating efficient processing and analysis of audio data |
| **Size**              | Typically one data point per channel per unit of time (e.g., one sample for left and one for right in stereo) | Contains multiple samples (e.g., 1024, 2048, 4096 samples) |
| **Usage**             | Fundamental building blocks of digital audio              | Used in signal processing, analysis, and encoding algorithms |
| **Time Resolution**   | High time resolution (depends on the sample rate, e.g., 44.1 kHz means 44,100 samples per second) | Lower time resolution compared to individual samples but higher frequency resolution |
| **Frequency Resolution** | Low frequency resolution (single sample doesn't provide frequency information) | Higher frequency resolution (more samples in a frame lead to better frequency analysis) |
| **Examples in Audio Processing** | Directly used in playback and basic audio manipulations | Used in FFT, audio compression (e.g., MP3), and other signal processing tasks |
| **Memory and Storage** | Each sample requires storage (e.g., 16-bit or 24-bit per sample) | Frames can be compressed or encoded, often reducing storage requirements |
| **Processing**        | Real-time playback and simple transformations             | Batch processing, often requiring buffering and more computational power |


### Some commands
`memcpy` - is memory ti memory copy. It is used to specify the range of characters which could not exceed the size of the source  memory.
```c
void *memcpy(void *dest, const void* src_str, size_t n)
```

### `gcc` Compilation Command wtih linking library

```bash
gcc -o fft fft.c -lm
```
- **`-lm`**: Tells the linker to include the math library (`libm`). This is necessary for functions like `sin()`, `cos()`, `sqrt()`, etc., which are defined in `math.h`.


```bash
clang -Wall -Wextra -o fft fft.c -lm
```
