#pragma once

/* Common files */
#include "exceptions.hpp"
#include "os/system_macros.hpp" 


#ifndef __COUNTER__
    #define __COUNTER__ __LINE__
#endif

#define PGR_NORETURN    [[noreturn]]
#define PGR_NODISCARD   [[nodiscard]]

#define ARRAY_SIZE(array) sizeof(array) / sizeof(array[0])

#define SELF std::remove_pointer<decltype(this)>::type
