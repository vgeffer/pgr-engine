#pragma once

/// @brief Main runtime class of the engine
#include "backends/backend.hpp"
#include "common/os/dynamic_loader.hpp"
#include <chrono>
#include <core/app/app_config.hpp>
#include <common/runtime/memory.hpp>

namespace pgreng::core::app {

    class application  {

        public:
            /// @brief Creates and initializes the runtime
            ///
            /// @param window Window to tie the runtime to
            application(const std::string& project_path);
            ~application();

            /// @brief Starts the mainloop
            int start(); 
            inline static float global_clock() noexcept { return s_instance->m_global_clock; }
            
        private:
            inline bool mainloop_tick(std::chrono::system_clock::time_point& tp);
            
        private:
            inline static application * s_instance = nullptr;
            float m_global_clock;           ///< Global clock
            
            app_config               m_config;
            backends::backend        m_backend;
            common::runtime::memory  m_memory;      
            common::os::dynamic_loader m_backend_handle;
            

    };
}