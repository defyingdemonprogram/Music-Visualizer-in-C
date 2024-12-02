#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <rlgl.h>

#include "plug.h"
#include "ffmpeg.h"
#define _WINDOWS_
#include "miniaudio.h"
#define NOB_IMPLEMENTATION
#include "nob.h"

#define N (1 << 15)
#define FONT_SIZE 69

#define RENDER_FPS 60
#define RENDER_FACTOR 120
#define RENDER_WIDTH (16 * RENDER_FACTOR)
#define RENDER_HEIGHT (9 * RENDER_FACTOR)

// Microsoft could not update their parser OMEGALUL
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/complex-math-support?view=msvc-170#types-used-in-complex-math
#ifdef _MSC_VER
#   define Float_Complex _Fcomplex
#   define cfromreal(re) _FCbuild(re, 0)
#   define cfromimag(im) _FCbuild(0, im)
#   define mulcc _FCmulcc
#   define addcc(a, b) _FCbuild(crealf(a) + crealf(b), cimagf(a) + cimagf(b))
#   define subcc(a, b) _FCbuild(crealf(a) - crealf(b), cimagf(a) - cimagf(b))
#else
#   define Float_Complex float complex
#   define cfromreal(re) (re)
#   define cfromimag(im) ((im)*I)
#   define mulcc(a, b) ((a)*(b))
#   define addcc(a, b) ((a)+(b))
#   define subcc(a, b) ((a)-(b))
#endif // _MSC_VER

// Struct Definitions
typedef struct {
    char *file_path;
    Music music;
} Sample;

typedef struct {
    Sample *items;
    size_t count;
    size_t capacity;
} Samples;

typedef struct {
    // Visualizer
    Samples samples;
    int current_sample;
    Font font;
    Shader circle;
    int circle_radius_location;
    int circle_power_location;

    // Renderer
    bool rendering;
    RenderTexture2D screen;
    Wave wave;
    float *wave_samples;
    size_t wave_cursor;
    FFMPEG *ffmpeg;

    // FFT Analyzer
    float in_raw[N];
    float in_win[N];
    Float_Complex out_raw[N];
    float out_log[N];
    float out_smooth[N];
    float out_smear[N];

    // Microphone
    bool capturing;
    ma_device *microphone;
} Plug;


Plug *p = NULL;

bool fft_settled(void) {
    float eps = 1e-3;
    for (size_t i = 0; i < N; ++i) {
        if (p->out_smooth[i] > eps || p->out_smear[i] > eps) return false;
    }
    return true;
}

void fft_clean(void) {
    memset(p->in_raw, 0, sizeof(p->in_raw));
    memset(p->in_win, 0, sizeof(p->in_win));
    memset(p->out_raw, 0, sizeof(p->out_raw));
    memset(p->out_log, 0, sizeof(p->out_log));
    memset(p->out_smooth, 0, sizeof(p->out_smooth));
    memset(p->out_smear, 0, sizeof(p->out_smear));
}

// FFT Implementation
void fft(float in[], size_t stride, Float_Complex out[], size_t n) {
    assert(n > 0);
    if (n == 1) {
        out[0] = cfromreal(in[0]);
        return;
    }

    fft(in, stride * 2, out, n / 2);
    fft(in + stride, stride * 2, out + n / 2, n / 2);

    for (size_t k = 0; k < n / 2; ++k) {
        float t = (float)k / n;
        Float_Complex v = mulcc(cexpf(cfromimag(-2  *PI * t)), out[k + n / 2]);
        Float_Complex e = out[k];
        out[k]          = addcc(e, v);
        out[k + n / 2]  = subcc(e, v);
    }
}

static inline float amp(Float_Complex z) {
    float a = fabsf(crealf(z));
    float b = fabsf(cimagf(z));
    return logf(a * a + b * b);
}

// FFT Analysis
size_t fft_analyze(float dt) {
    for (size_t i = 0; i < N; ++i) {
        float t = (float)i / (N - 1);
        float hann = 0.5 - 0.5 * cosf(2 *PI * t);
        p->in_win[i] = p->in_raw[i] * hann;
    }

    fft(p->in_win, 1, p->out_raw, N);
    
    // "Squash" into the Logarithmic Scale
    float step = 1.06;
    float lowf = 1.0f;
    size_t m = 0;
    float max_amp = 1.0f;
    
    for (float f = lowf; (size_t)f < N / 2; f = ceilf(f * step)) {
        float f1 = ceilf(f * step);
        float a = 0.0f;
        for (size_t q = (size_t)f; q < N / 2 && q < (size_t)f1; ++q) {
            float b = amp(p->out_raw[q]);
            if (b > a) a = b;
        }
        if (max_amp < a) max_amp = a;
        p->out_log[m++] = a;
    }

    // Normalize Frequencies to 0..1 range
    for (size_t i = 0; i < m; ++i) {
        p->out_log[i] /= max_amp;
    }

    // Smooth and Smear Values
    float smoothness = 8;
    float smearness = 3;
    for (size_t i = 0; i < m; ++i) {
        p->out_smooth[i] += (p->out_log[i] - p->out_smooth[i]) * smoothness * dt;
        p->out_smear[i] += (p->out_smooth[i] - p->out_smear[i]) * smearness * dt;
    }
    return m;
}

// FFT Rendering
void fft_render(Rectangle boundary, size_t m) {
    // The width of a single bar
    float cell_width = boundary.width / m;

    // Global color parameters
    float saturation = 0.75f;
    float value = 1.0f;

    // Render Bars
    for (size_t i = 0; i < m; ++i) {
        float hue = (float)i / m;
        float t = p->out_smooth[i];
        Color color = ColorFromHSV(hue * 360, saturation, value);
        Vector2 startPos = {
            boundary.x + i*cell_width + cell_width / 2,
            boundary.y + boundary.height - boundary.height*2/3*t,
        };
        Vector2 endPos = {
            boundary.x + i*cell_width + cell_width/2,
            boundary.y + boundary.height,
        };
        float thick = cell_width / 3 * sqrtf(t);
        DrawLineEx(startPos, endPos, thick, color);
    }

    // Render Smears
    Texture2D texture = {rlGetTextureIdDefault(), 1, 1, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
    SetShaderValue(p->circle, p->circle_radius_location, (float[1]){0.3f}, SHADER_UNIFORM_FLOAT);
    SetShaderValue(p->circle, p->circle_power_location, (float[1]){3.0f}, SHADER_UNIFORM_FLOAT);
    BeginShaderMode(p->circle);
    for (size_t i = 0; i < m; ++i) {
        float start = p->out_smear[i];
        float end = p->out_smooth[i];
        float hue = (float)i / m;
        Color color = ColorFromHSV(hue * 360, saturation, value);
        Vector2 startPos = {
            boundary.x + i*cell_width + cell_width/2,
            boundary.y + boundary.height - boundary.height*2/3*start,
        };
        Vector2 endPos = {
            boundary.x + i*cell_width + cell_width/2,
            boundary.y + boundary.height - boundary.height*2/3*end,
        };
        float radius = cell_width * 3 * sqrtf(end);
        Vector2 origin = {0};
        
        if (endPos.y >= startPos.y) {
            Rectangle dest = {
                .x = startPos.x - radius / 2, 
                .y = startPos.y, 
                .width = radius, 
                .height = endPos.y - startPos.y
            };
            Rectangle source = {0, 0, 1, 0.5};
            DrawTexturePro(texture, source, dest, origin, 0, color);
        } else {
            Rectangle dest = {
                .x = endPos.x - radius / 2, 
                .y = endPos.y, 
                .width = radius, 
                .height = startPos.y - endPos.y
            };
            Rectangle source = {0, 0.5, 1, 0.5};
            DrawTexturePro(texture, source, dest, origin, 0, color);
        }
    }
    EndShaderMode();

    // Render Circles
    SetShaderValue(p->circle, p->circle_radius_location, (float[1]){0.07f}, SHADER_UNIFORM_FLOAT);
    SetShaderValue(p->circle, p->circle_power_location, (float[1]){5.0f}, SHADER_UNIFORM_FLOAT);
    BeginShaderMode(p->circle);
    for (size_t i = 0; i < m; ++i) {
        float t = p->out_smooth[i];
        float hue = (float)i / m;
        Color color = ColorFromHSV(hue * 360, saturation, value);
        Vector2 center = {
            boundary.x + i*cell_width + cell_width/2,
            boundary.y + boundary.height - boundary.height*2/3*t,
        };
        float radius = cell_width * 6 * sqrtf(t);
        Vector2 position = { .x = center.x - radius, .y = center.y - radius };
        DrawTextureEx(texture, position, 0, 2 * radius, color);
    }
    EndShaderMode();
}

// Push Frame Data
void fft_push(float frame) {
    memmove(p->in_raw, p->in_raw + 1, (N - 1) * sizeof(p->in_raw[0]));
    p->in_raw[N - 1] = frame;
}

// Audio Callback
void callback(void* bufferData, unsigned int frames) {
    // https://cdecl.org/?q=float+%28*fs%29%5B2%5D
    float(*fs)[2] = bufferData; // Treating music as 2 channels
    for (size_t i = 0; i < frames; ++i) {
        fft_push(fs[i][0]);
    }
}

void ma_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
    callback((void*)pInput, frameCount);
    (void)pOutput;
    (void)pDevice;
}

// Plug Initialization
void plug_init() {
    p = malloc(sizeof(*p));
    assert(p != NULL && "Buy More RAM since it is insufficient");
    memset(p, 0, sizeof(*p));

    p->font = LoadFontEx("./resources/fonts/Alegreya-Regular.ttf", FONT_SIZE, NULL, 0);
    p->circle = LoadShader(NULL, "./resources/shaders/circle.fs");
    p->circle_radius_location = GetShaderLocation(p->circle, "radius");
    p->circle_power_location = GetShaderLocation(p->circle, "power");
    p->screen = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);
    p-> current_sample = -1;
}

// Pre-reload Function
Plug *plug_pre_reload(void) {
    for (size_t i = 0; i < p->samples.count; ++i) {
        Sample *it = &p->samples.items[i];
        DetachAudioStreamProcessor(it->music.stream, callback);
    }
    return p;
}

// Post-reload Function
void plug_post_reload(Plug *prev) {
    p = prev;
    for (size_t i=0; i< p->samples.count; ++i) {
        Sample *it = &p->samples.items[i];
        AttachAudioStreamProcessor(it->music.stream, callback);
    }
    UnloadShader(p->circle);
    p->circle = LoadShader(NULL, "./resources/shaders/circle.fs");
    p->circle_radius_location = GetShaderLocation(p->circle, "radius");
    p->circle_power_location = GetShaderLocation(p->circle, "power");
}

Sample *current_sample(void) {
    if (0 <= p->current_sample && (size_t) p->current_sample < p->samples.count) {
        return &p->samples.items[p->current_sample];
    }
    return NULL;
}

void error_load_file_popup(void) {
    // TODO: Implement the popup that indicates that we could not load the file
    TraceLog(LOG_ERROR, "Could not load file");
}

// Main Update Function
void plug_update(void) {
    int w = GetRenderWidth();
    int h = GetRenderHeight();

    BeginDrawing();
    ClearBackground(GetColor(0x151515FF));

    if (!p->rendering) { // We are in the Preview Mode
        if (p->capturing) {
            if (p->microphone != NULL) {
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_M)) {
                    ma_device_uninit(p->microphone);
                    p->microphone = NULL;
                    p->capturing = false;
                }

                size_t m = fft_analyze(GetFrameTime());
                fft_render(CLITERAL(Rectangle) {
                    0, 0, GetRenderWidth(), GetRenderHeight()
                }, m);
            } else {
                if (IsKeyPressed(KEY_ESCAPE)) {
                    p->capturing = false;
                }

                const char *label = "Capture Device Error: Check the Logs";
                Color color = RED;
                int fontSize = p->font.baseSize;
                Vector2 size = MeasureTextEx(p->font, label, fontSize, 0);
                Vector2 position = {
                    w/2 - size.x/2,
                    h/2 - size.y/2,
                };
                DrawTextEx(p->font, label, position, fontSize, 0, color);

                label = "(Press ESC to Continue)";
                fontSize = p->font.baseSize*2/3;
                size = MeasureTextEx(p->font, label, fontSize, 0);
                position.x = w/2 - size.x/2,
                position.y = h/2 - size.y/2 + fontSize,
                DrawTextEx(p->font, label, position, fontSize, 0, color);
            }
        } else {
            if (IsFileDropped()) {
                FilePathList droppedFiles = LoadDroppedFiles();
                for (size_t i = 0; i < droppedFiles.count; ++i) {
                    char *file_path = strdup(droppedFiles.paths[i]);
                    
                    Sample *sample = current_sample();
                    if (sample) StopMusicStream(sample->music);

                    Music music = LoadMusicStream(file_path);
                    if (IsMusicReady(music)) {
                        printf("music.frameCount = %u\n", music.frameCount);
                        printf("music.stream.sampleRate = %u\n", music.stream.sampleRate);
                        printf("music.stream.sampleSize = %u\n", music.stream.sampleSize);
                        printf("music.stream.channels = %u\n", music.stream.channels);
                        SetMusicVolume(music, 0.5f);
                        AttachAudioStreamProcessor(music.stream, callback);
                        PlayMusicStream(music);

                        nob_da_append(&p->samples, (CLITERAL(Sample) {
                            .file_path = file_path,
                            .music = music,
                        }));
                        p->current_sample = p->samples.count - 1;
                    } else {
                        free(file_path);
                        error_load_file_popup();
                    }
                }
                UnloadDroppedFiles(droppedFiles);
            }
            if (IsKeyPressed(KEY_M)) {
                // TODO: let the user choose their mic
                ma_device_config deviceConfig = ma_device_config_init(ma_device_type_capture);
                deviceConfig.capture.format = ma_format_f32;
                deviceConfig.capture.channels = 2;
                deviceConfig.sampleRate = 44100;
                deviceConfig.dataCallback = ma_callback;
                deviceConfig.pUserData = NULL;
                p->microphone = malloc(sizeof(ma_device));
                assert(p->microphone != NULL && "Buy More RAM lol!!");
                ma_result result = ma_device_init(NULL, &deviceConfig, p->microphone);
                if (result != MA_SUCCESS) {
                    TraceLog(LOG_ERROR, "MINIAUDIO: Failed to initialize capture device: %s", ma_result_description(result));
                }
                if (p->microphone != NULL) {
                    ma_result result = ma_device_start(p->microphone);
                    if (result != MA_SUCCESS) {
                        TraceLog(LOG_ERROR, "MINIAUDIO: Failed to start device: %s", ma_result_description(result));
                        ma_device_uninit(p->microphone);
                        p->microphone = NULL;
                    }
                }
                p->capturing = true;
            }

            Sample *sample = current_sample();
            if (sample) { // The music is loaded and ready
                UpdateMusicStream(sample->music);

                if (IsKeyPressed(KEY_SPACE)) {
                    if (IsMusicStreamPlaying(sample->music)) {
                        PauseMusicStream(sample->music);
                    } else {
                        ResumeMusicStream(sample->music);
                    }
                }

                if (IsKeyPressed(KEY_W)) {
                    StopMusicStream(sample->music);
                    PlayMusicStream(sample->music);
                }

                if (IsKeyPressed(KEY_F)) {
                    StopMusicStream(sample->music);

                    fft_clean();
                    // TODO: LoadWave is pretty slow on big files
                    p->wave = LoadWave(sample->file_path);
                    p->wave_cursor = 0;
                    p->wave_samples = LoadWaveSamples(p->wave);
                    p->ffmpeg = ffmpeg_start_rendering(p->screen.texture.width, p->screen.texture.height, RENDER_FPS, sample->file_path);
                    p->rendering = true;
                    SetTraceLogLevel(LOG_WARNING);
                }

                float panel_height = h*0.25;
                Rectangle preview_boundary = {
                    0, 0, w, h - panel_height
                };

                size_t m = fft_analyze(GetFrameTime());
                fft_render(preview_boundary, m);

                static float panel_scroll = 0;
                static float panel_velocity = 0;
                panel_velocity *= 0.9;
                panel_velocity += GetMouseWheelMove()*panel_height*4;
                panel_scroll += panel_velocity*GetFrameTime();
                Rectangle panel_boundary = {
                    .x = panel_scroll,
                    .y = preview_boundary.height,
                    .width = w,
                    .height = panel_height
                };
                float panel_padding = panel_height*0.1;

                for (size_t i = 0; i < p->samples.count; ++i) {
                    Rectangle item_boundary = {
                        .x = i*panel_height + panel_boundary.x + panel_padding,
                        .y = panel_boundary.y + panel_padding,
                        .width = panel_height - panel_padding *2,
                        .height = panel_height - panel_padding*2,
                    };
                    if (((int) i != p->current_sample)) {
                        if (CheckCollisionPointRec(GetMousePosition(), item_boundary)) {
                            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                                Sample *sample = current_sample();
                                if (sample) StopMusicStream(sample->music);
                                PlayMusicStream(p->samples.items[i].music);
                                p->current_sample = i;
                            }
                            DrawRectangleRec(item_boundary, RED);
                        } else {
                            DrawRectangleRec(item_boundary, WHITE);
                        }
                    } else {
                        DrawRectangleRec(item_boundary, BLUE);
                    }
                }
            } else { // We are waiting for the user to Drag&Drop the Music
                const char *label = "Drag&Drop Music Here";
                Color color = WHITE;
                Vector2 size = MeasureTextEx(p->font, label, p->font.baseSize, 0);
                Vector2 position = { w / 2 - size.x / 2, h / 2 - size.y / 2 };
                DrawTextEx(p->font, label, position, p->font.baseSize, 0, color);
            }
        }
    } else { // We are in the Rendering Mode
    Sample *sample = current_sample();
    NOB_ASSERT(sample != NULL);
        if (p->ffmpeg == NULL) { // Starting FFmpeg process has failed for some reason
            if (IsKeyPressed(KEY_ESCAPE)) {
                SetTraceLogLevel(LOG_INFO);
                UnloadWave(p->wave);
                UnloadWaveSamples(p->wave_samples);
                p->rendering = false;
                fft_clean();
                PlayMusicStream(sample->music);
            }

            const char *label = "FFmpeg Failure: Check the Logs";
            Color color = RED;
            int fontSize = p->font.baseSize;
            Vector2 size = MeasureTextEx(p->font, label, fontSize, 0);
            Vector2 position = {
                w/2 - size.x/2,
                h/2 - size.y/2,
            };
            DrawTextEx(p->font, label, position, fontSize, 0, color);

            label = "(Press ESC to Continue)";
            fontSize = p->font.baseSize * 2/3;
            size = MeasureTextEx(p->font, label, fontSize, 0);
            position.x = w/2 - size.x/2;
            position.y = h/2 - size.y/2 + fontSize;
            DrawTextEx(p->font, label, position, fontSize, 0, color);
        } else { // FFmpeg process is going
            if ((p->wave_cursor >= p->wave.frameCount && fft_settled()) || IsKeyPressed(KEY_ESCAPE)) { // Rendering is finished or cancelled
                // TODO: ffmpeg processes frames slower than we generate them
                // So when we cancel the rendering ffmpeg is still going and blocking the UI
                // We need to do something about that. For example inform the user that
                // we are finalizing the rendering or something.
                if (!ffmpeg_end_rendering(p->ffmpeg)) {
                    // NOTE: Ending FFmpeg process has failed, let's mark ffmpeg handle as NULL
                    // which will be interpreted as "FFmpeg Failure" on the next frame.
                    //
                    // It should be safe to set ffmpeg to NULL even if ffmpeg_end_rendering() failed
                    // cause it should deallocate all the resources even in case of a failure.
                    p->ffmpeg = NULL;
                } else {
                    SetTraceLogLevel(LOG_INFO);
                    UnloadWave(p->wave);
                    UnloadWaveSamples(p->wave_samples);
                    p->rendering = false;
                    fft_clean();
                    PlayMusicStream(sample->music);
                }
            } else { // Rendering is going...
            const char *label = "Rendering video...";
            Color color = WHITE;

            Vector2 size = MeasureTextEx(p->font, label, p->font.baseSize, 0);
            Vector2 position = { w / 2 - size.x / 2, h / 2 - size.y / 2 };
            DrawTextEx(p->font, label, position, p->font.baseSize, 0, color);

            // Progress bar
            float bar_width = w*2/3;
            float bar_height = p->font.baseSize*0.25;
            float bar_progress = (float)p->wave_cursor/p->wave.frameCount;
            float bar_padding_top = p->font.baseSize*0.5;
            if (bar_progress > 1) bar_progress = 1;
            Rectangle bar_filling = {
                .x = w/2 - bar_width/2,
                .y = h/2 + p->font.baseSize/2 + bar_padding_top,
                .width = bar_width*bar_progress,
                .height = bar_height,
            };
            DrawRectangleRec(bar_filling, WHITE);

            Rectangle bar_box = {
                .x = w/2 - bar_width/2,
                .y = h/2 + p->font.baseSize/2 + bar_padding_top,
                .width = bar_width,
                .height = bar_height,
            };
            DrawRectangleLinesEx(bar_box, 2, WHITE);

            // Rendering
            size_t chunk_size = p->wave.sampleRate / RENDER_FPS;
            {
                float *fs = (float*)p->wave_samples;
                for (size_t i = 0; i < chunk_size; ++i) {
                    if (p->wave_cursor < p->wave.frameCount) {
                        fft_push(fs[p->wave_cursor*p->wave.channels + 0]);
                    } else {
                        fft_push(0);
                    }
                    p->wave_cursor += 1;
                }
            }

            size_t m = fft_analyze(1.0f / RENDER_FPS);

            BeginTextureMode(p->screen);
            ClearBackground(GetColor(0x151515FF));
            fft_render(CLITERAL(Rectangle) {
                0, 0, p->screen.texture.width, p->screen.texture.height
            }, m);
            EndTextureMode();

            Image image = LoadImageFromTexture(p->screen.texture);
            if (!ffmpeg_send_frame_flipped(p->ffmpeg, image.data, image.width, image.height)) {
                ffmpeg_end_rendering(p->ffmpeg);
                p->ffmpeg = NULL;
            }
            UnloadImage(image);
            }
        }
    }
    EndDrawing();
}
