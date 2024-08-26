#include <stdio.h>
#include <string.h>
#include <raylib.h>

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

__int16_t global_frames[1024] = {0};
size_t global_frames_count = 0;


void callback (void *bufferData, unsigned int frames) {
    if (frames > ARRAY_LEN(global_frames)/2) {
        frames = ARRAY_LEN(global_frames) /2;
    }
    memcpy(global_frames, bufferData, frames*sizeof(__int16_t)*2);
    global_frames_count = frames;
}

int main() {
    // initiate window
    InitWindow(800, 700, "Music Visualizer");
    SetTargetFPS(600);

    // Initialize audio device
    InitAudioDevice();

    // Load sound
    // Sound is loaded all at once since it has instantaneous frequency(short time)
    // Music is loaded in stream since we want to play music in backgroud. its duration is longer
    Music music = LoadMusicStream("videoplayback.mp3");
    // Sound sound = LoadSound("videoplayback.mp3");
    // PlaySound(sound);  // Sound is played in separate thread
    // Since music is opened but not loaded in memory, we need event loop to play with it
    PlayMusicStream(music);
    printf("music.stream.sampleRate: %d\n", music.stream.sampleRate);
    printf("music.stream.sample: %d\n", music.stream.sampleSize);
    printf("music.stream.channels: %d\n", music.stream.channels);
    printf("music.frameCount: %d\n", music.frameCount);

    // set volume of music
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

        if (IsKeyPressed(KEY_Q))
            break;

        BeginDrawing();
        char text[50];
        sprintf(text, "Music FrameCount: %d", music.frameCount);

        float cell_width = (float) GetRenderWidth() / global_frames_count;
        for (size_t i=0; i<global_frames_count; i++) {
            __int16_t sample = global_frames[i];
            printf("%d\n", sample);
            DrawRectangle(i*cell_width, GetRenderHeight()/2, 1, 10, RED);
        }
        DrawText(text, 10, 10, 5, BLUE);

        ClearBackground(CLITERAL(Color) {0x18, 0x18, 0x18, 0xFF});
        EndDrawing();
    }
    // sleep(5);
	printf("Hello World\n");
	return 0;
}
