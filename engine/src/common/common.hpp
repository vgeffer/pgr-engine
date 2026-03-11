#pragma once
#include "exceptions.hpp"

/* Code candy */
#define IN
#define OUT

#ifndef __COUNTER__
    #define __COUNTER__ __LINE__
#endif

#define ARRAY_SIZE(array) sizeof(array) / sizeof(array[0])

#define SELF std::remove_pointer<decltype(this)>::type
