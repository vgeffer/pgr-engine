#pragma once
#include "system_macros.hpp"

#if defined(PLATFORM_WINDOWS)
    #include <Windows.h>
#endif

namespace pgreng::common::os {
    
    struct dynamic_loader {

        #if defined(PLATFORM_POSIX)
            using library_handle = void*;
        #elif defined(PLATFORM_WINDOWS)
            using library_handle = HINSTANCE;
        #endif
    
    
        template <typename T, typename... Args>
        T(*func_addr(const char*))(Args...) { }
    };
}