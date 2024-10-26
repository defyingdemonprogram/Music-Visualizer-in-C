// NOTE: In case you can't include the entire raylib.h (for instance when you also want to include windows.h)
// but still need its logging capabilities, just include this raylib_log.h file.

#ifndef RAYLIB_LOG_H_
#define RAYLIB_LOG_H_

// Function specifiers for building/using the library as a shared library (Windows)
// NOTE: Microsoft specifiers to indicate that symbols are imported/exported from a .dll
#if defined(_WIN32)
    #if defined(BUILD_LIBTYPE_SHARED)
        #ifndef __TINYC__
            #define RLAPI __declspec(dllexport) // Building as a Win32 shared library (.dll)
        #else
            #define __declspec(x) __attribute__((x))
            #define RLAPI __declspec(dllexport) // For TinyCC compatibility
        #endif
    #elif defined(USE_LIBTYPE_SHARED)
        #define RLAPI __declspec(dllimport) // Using as a Win32 shared library (.dll)
    #endif
#endif

#ifndef RLAPI
    #define RLAPI // Functions defined as 'extern' by default (implicit specifiers)
#endif

// Trace log levels organized by priority
typedef enum {
    LOG_ALL = 0,        // Display all logs
    LOG_TRACE,          // Trace logging, intended for internal use only
    LOG_DEBUG,          // Debug logging, used for internal debugging (should be disabled in release builds)
    LOG_INFO,           // Info logging, for program execution information
    LOG_WARNING,        // Warning logging, for recoverable failures
    LOG_ERROR,          // Error logging, for unrecoverable failures
    LOG_FATAL,          // Fatal logging, to abort program execution: exit(EXIT_FAILURE)
    LOG_NONE            // Disable logging
} TraceLogLevel;

RLAPI void TraceLog(int logLevel, const char *text, ...);

#endif // RAYLIB_LOG_H_
