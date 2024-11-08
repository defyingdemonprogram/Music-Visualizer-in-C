#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

typedef enum {
    TARGET_LINUX,
    TARGET_WIN32,
} Target;

const char *target_show(Target target) {
    if (target == TARGET_LINUX) return "TARGET_LINUX";
    if (target == TARGET_WIN32) return "TARGET_WIN32";
    NOB_ASSERT(0 && "unreachable");
    return "(unknown)";
}

void target_compiler(Nob_Cmd *cmd, Target target) {
    if (target == TARGET_WIN32) {
        nob_cmd_append(cmd, "x86_64-w64-mingw32-gcc");
    } else {
        nob_cmd_append(cmd, "clang");
    }
}

void cflags(Nob_Cmd *cmd, Target target) {
    nob_cmd_append(cmd, "-Wall", "-Wextra", "-ggdb");
    if (target == TARGET_WIN32) {
        nob_cmd_append(cmd, "-I./raylib-5.0_win64_mingw-w64/include");
    } else {
        nob_cmd_append(cmd, "-I./build/");
    }
}

void musializer_src(Nob_Cmd *cmd, Target target) {
    nob_cmd_append(cmd, "./src/main.c");
    nob_cmd_append(cmd, "./src/plug.c");
    nob_cmd_append(cmd, "./src/separate_translation_unit_for_miniaudio.c");
    if (target == TARGET_WIN32) {
        nob_cmd_append(cmd, "./src/ffmpeg_windows.c");
    } else {
        nob_cmd_append(cmd, "./src/ffmpeg_linux.c");
    }
}

void link_libraries(Nob_Cmd *cmd, Target target) {
    if (target == TARGET_WIN32) {
        nob_cmd_append(cmd, "-L./raylib-5.0_win64_mingw-w64/lib/");
        nob_cmd_append(cmd, "-lraylib", "-lwinmm", "-lgdi32", "-static");
    } else {
        nob_cmd_append(cmd, "-L./build/");
        nob_cmd_append(cmd, "-lraylib", "-lm", "-ldl", "-lpthread");
    }
}

bool build_musializer_executable(const char *output_path, Target target) {
    Nob_Cmd cmd = {0};

    target_compiler(&cmd, target);
    cflags(&cmd, target);
    nob_cmd_append(&cmd, "-o", "./build/musializer");
    musializer_src(&cmd, target);
    link_libraries(&cmd, target);

    bool result = nob_cmd_run_sync(cmd);
    nob_cmd_free(cmd);
    return result;
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    const char *program = nob_shift_args(&argc, &argv);

    if (argc <= 0) {
        nob_log(NOB_INFO, "Usage: %s <subcommand>", program);
        nob_log(NOB_INFO, "Subcommands:");
        nob_log(NOB_INFO, "    build");
        nob_log(NOB_INFO, "    logo");
        nob_log(NOB_ERROR, "No subcommand is provided");
        return 1;
    }

    const char *subcommand = nob_shift_args(&argc, &argv);

    if (strcmp(subcommand, "build") == 0) {
#ifdef _WIN32
        Target target = TARGET_WIN32;
#else
        Target target = TARGET_LINUX;
#endif

        if (argc > 0) {
            const char *subcmd = nob_shift_args(&argc, &argv);
            if (strcmp(subcmd, "win32") == 0) {
                target = TARGET_WIN32;
            } else if (strcmp(subcmd, "linux") == 0) {
                target = TARGET_LINUX;
            } else {
                fprintf(stderr, "[ERROR] unknown subcommand %s\n", subcmd);
                return 1;
            }
        }

        nob_log(NOB_INFO, "TARGET: %s", target_show(target));
        if (!nob_mkdir_if_not_exists("build")) return 1;
        build_musializer_executable("./build/musializer", target);
        if (target == TARGET_WIN32) {
            if (!nob_copy_file("musializer-logged.bat", "build/musializer-logged.bat")) return 1;
        }
        if (!nob_copy_directory_recursively("./resources/", "./build/resources/")) return 1;
    } else if (strcmp(subcommand, "logo") == 0) {
        Nob_Cmd cmd = {0};
        nob_cmd_append(&cmd, "convert");
        nob_cmd_append(&cmd, "-background", "None");
        nob_cmd_append(&cmd, "./resources/logo/logo.svg");
        nob_cmd_append(&cmd, "-resize", "256");

        nob_cmd_append(&cmd, "./resources/logo/logo-256.ico");
        if (!nob_cmd_run_sync(cmd)) return 1;

        cmd.count -= 1;

        nob_cmd_append(&cmd, "./resources/logo/logo-256.png");
        if (!nob_cmd_run_sync(cmd)) return 1;
    }

    return 0;
}
