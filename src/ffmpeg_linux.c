#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h> // Include for strerror

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

int ffmpeg_start_rendering(size_t width, size_t height, size_t fps, const char *sound_file_path) {
    int pipefd[2];

    if (pipe(pipefd) < 0) {
        fprintf(stderr, "ERROR: Could not create a pipe: %s\n", strerror(errno));
        return -1;
    }

    pid_t child = fork();
    if (child < 0) {
        fprintf(stderr, "ERROR: Could not fork a child: %s\n", strerror(errno));
        return -1;
    }

    if (child == 0) {
        if (dup2(pipefd[READ_END], STDIN_FILENO) < 0) {
            fprintf(stderr, "ERROR: Could not reopen read end of pipe as stdin: %s\n", strerror(errno));
            return -1;
        }
        close(pipefd[WRITE_END]); // Close the write end in the child process

        char resolution[64];
        snprintf(resolution, sizeof(resolution), "%zux%zu", width, height);
        char framerate[64];
        snprintf(framerate, sizeof(framerate), "%zu", fps);

        int ret = execlp("ffmpeg",
            "ffmpeg",
            "-loglevel", "verbose",
            "-y",

            "-f", "rawvideo",
            "-pix_fmt", "rgba",
            "-s", resolution,
            "-r", framerate,
            "-i", "-",
            "-i", sound_file_path,

            "-c:v", "libx264",
            "-vb", "2500k",
            "-c:a", "aac",
            "-ab", "200k",
            "-pix_fmt", "yuv420p",
            "output.mp4",

            NULL
        );
        if (ret < 0) {
            fprintf(stderr, "ERROR: Could not run ffmpeg as a child process: %s\n", strerror(errno)); // Fix typo: strrror -> strerror
            return -1;
        }
        assert(0 && "Unreachable");
    }
    close(pipefd[READ_END]); // Close the read end in the parent process

    return pipefd[WRITE_END];
}

void ffmpeg_end_rendering(int pipe) {
    close(pipe);
    wait(NULL); 
}

void ffmpeg_send_frame_flipped(int pipe, void *data, size_t width, size_t height) {
    for (size_t y = height; y > 0; --y) {
        write(pipe, (uint32_t*)data + (y - 1) * width, sizeof(uint32_t) * width);
    }
}
