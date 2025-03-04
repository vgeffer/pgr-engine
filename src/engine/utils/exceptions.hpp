#pragma once
#include <stdexcept>

namespace exceptions {

    class shader_compile_error : public std::runtime_error { };
    class shader_link_error : public std::runtime_error { };
    class shader_uniform_error : public std::runtime_error { };
    class resource_loading_error : public std::runtime_error { };    
}
