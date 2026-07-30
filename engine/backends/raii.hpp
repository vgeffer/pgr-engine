#pragma once
#include <utility>
#include <common/common.hpp>
#include <common/runtime/memory.hpp>

#define INCLUDE_TYPES
#define EXPORT_FUNCTION_PROTOTYPES
#include <backends/backend/backend_functions.hpp>

namespace pgreng::backends::raii { //todo: figure out essentially

    template <typename Handle, auto Constructor, auto Destructor>
    struct raii_wrapper {
        using handle_type = Handle;

        template <typename... Args>
        raii_wrapper(Args... args) { Constructor(m_handle, std::forward<Args>(args)...); }
        ~raii_wrapper() { Destructor(m_handle); }

        operator handle_type&() const noexcept { return m_handle; }
        private:
            Handle m_handle;
    };

    typedef raii_wrapper<hwindow, win_create, win_destroy> window;    
};