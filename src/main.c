#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#include <raylib.h>

#define N 256
#define ARRAY_LEN(xs) (sizeof(xs)/sizeof(xs[0]))

float pi;

float in[N];
float complex out[N];
float max_amp;

typedef struct {
    float left;
    float right;
} Frame;

// Ported from https://rosettacode.org/wiki/Fast_Fourier_transform#Python
void fft(float in[], size_t stride, float complex out[], size_t n) {
    assert(n > 0);

    if (n == 1) {
        out[0] = in[0];
        return;
    }

    fft(in, stride * 2, out, n / 2);
    fft(in + stride, stride * 2, out + n / 2, n / 2);

    for (size_t k = 0; k < n / 2; ++k) {
        float t = (float)k / n;
        float complex v = cexp(-2 * I * pi * t) * out[k + n / 2];
        float complex e = out[k];
        out[k]       = e + v;
        out[k + n / 2] = e - v;
    }
}

float amp(float complex z) {
    float a = fabsf(crealf(z));
    float b = fabsf(cimagf(z));
    return (a > b) ? a : b;
}

void callback(void *bufferData, unsigned int frames) {
    if (frames < N) return;

    Frame *fs = bufferData;

    for (size_t i = 0; i < N; ++i) {
        in[i] = (i < frames) ? fs[i].left : 0.0f;
    }
    
    fft(in, 1, out, N);

    max_amp = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        float a = amp(out[i]);
        if (max_amp < a) max_amp = a;
    }
}

char *shift_args(int *argc, char ***argv) {
    assert(*argc > 0);
    char *result = (**argv);
    (*argv) += 1;
    (*argc) -= 1;
    return result;
}

int main(int argc, char **argv) {
    pi = atan2f(1, 1) * 4;
    const char *program = shift_args(&argc, &argv);
    
    if (argc == 0) {
        fprintf(stderr, "Usage: %s <input>\n", program);
        fprintf(stderr, "ERROR: no input file is provided\n");
        return 1;
    }
    const char *file_path = shift_args(&argc, &argv);

    InitWindow(800, 600, "Musializer");
    SetTargetFPS(60);

    InitAudioDevice();

    Music music = LoadMusicStream(file_path);
    if (music.stream.sampleSize != 16 && music.stream.sampleSize != 32) {
        fprintf(stderr, "ERROR: Unsupported sample size\n");
        return 1;
    }
    if (music.stream.channels != 2) {
        fprintf(stderr, "ERROR: Unsupported number of channels\n");
        return 1;
    }
    
    PlayMusicStream(music);
    SetMusicVolume(music, 0.5f);
    AttachAudioStreamProcessor(music.stream, callback);

    while (!WindowShouldClose()) {
        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_SPACE)) {
            if (IsMusicStreamPlaying(music)) {
                PauseMusicStream(music);
            } else {
                ResumeMusicStream(music);
            }
        }
        if (IsKeyPressed(KEY_Q)) {
            break;
        }

        int w = GetScreenWidth();
        int h = GetScreenHeight();

        BeginDrawing();
        ClearBackground(CLITERAL(Color) {0x18, 0x18, 0x18, 0xFF});
        char text[50];
        sprintf(text, "Music FrameCount: %d", music.frameCount);

        float cell_width = (float)w / N;
        for (size_t i = 0; i < N; ++i) {
            float t = (max_amp > 0) ? amp(out[i]) / max_amp : 0;
            DrawRectangle(i * cell_width, h / 2 - h / 2 * t, cell_width, h / 2 * t, RED);
        }
        EndDrawing();
        DrawText(text, 10, 10, 20, BLUE);
        ClearBackground(CLITERAL(Color) {0x18, 0x18, 0x18, 0xFF});
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
