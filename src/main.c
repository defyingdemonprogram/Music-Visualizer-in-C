#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <complex.h>
#include <raylib.h>

#ifndef _WIN32
#include <signal.h>
#endif // _WIN32

#include "plug.h"

const char *libplug_file_name = "libplug.so";
void *libplug = NULL;

#ifdef HOTRELOAD
#define PLUG(name, ...) name##_t *name=NULL;
#else
#define PLUG(name, ...) name##_t name;
#endif
LIST_OF_PLUGS
#undef PLUG

#ifdef HOTRELOAD
#include <dlfcn.h>
bool reload_libplug(void)
{
    if (libplug != NULL) dlclose(libplug);

    libplug = dlopen(libplug_file_name, RTLD_NOW);
    if (libplug == NULL) {
        fprintf(stderr, "ERROR: could not load %s: %s\n", libplug_file_name, dlerror());
        return false;
    }

    #define PLUG(name, ...) \
        name = dlsym(libplug, #name); \
        if (name == NULL) { \
            fprintf(stderr, "ERROR: could not find %s symbol in %s: %s\n", \
                    #name, libplug_file_name, dlerror()); \
            return false; \
        }
    LIST_OF_PLUGS
    #undef PLUG

    return true;
}
#else
#define reload_libplug() true
#endif
int main()
{
    #ifndef _WIN32
        struct sigaction act = {0};
        act.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &act, NULL);
    #endif // _WIN32
    if (!reload_libplug()) return 1;

    size_t factor = 60;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(factor*16, factor*9, "Musializer");
    SetTargetFPS(60);
    InitAudioDevice();

    plug_init();
    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE)) {
        if (IsKeyPressed(KEY_Q)) {
            CloseWindow();
        }
        if (IsKeyPressed(KEY_R)) {
            void *state = plug_pre_reload();
            if (!reload_libplug()) return 1;
            plug_post_reload(state);
        }
        plug_update();
    }

    return 0;
}
