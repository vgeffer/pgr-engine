#pragma once

/* Platform Resolution Macros */
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)

    #define PLATFORM_WINDOWS
    #if defined(PGR_EXPORT)
        #define PGRAPI __declspec(dllexport) 
    #else
        #define PGRAPI __declspec(dllimport) 
    #endif
#elif defined(__unix__) || defined(__linux__) || defined(__FreeBSD__)

    #define PLATFORM_UNIX
    #define PLATFORM_POSIX
    #define PGRAPI

#elif defined(__APPLE__) || defined(__MACH__)

    #define PLATFORM_MACOS
    #define PLATFORM_POSIX
    #define PGRAPI

#else
    #error Unknown or Unsupported Platform
#endif



/* Arch Resolution Macros */
#if defined(__x86_64__) || defined(_M_X64)
    #define ARCH_x86_64
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    #define ARCH_x86
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define ARCH_ARM64
#else
    #error Unknown or Unsupported CPU architecture
#endif