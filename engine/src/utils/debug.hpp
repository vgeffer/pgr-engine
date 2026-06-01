#pragma once

#include "vulkan/instance.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include <array>
#include <iostream>
#include <ostream>
#include <vulkan/vulkan.hpp>

namespace utils {
    
    class debug {

        public:
            enum class level : uint8_t {
                PROJECT_RELEASE = 0,    /* Least verbose logs, only errors reported */
                PROJECT_DEBUG   = 1,      /* Errors and warnings reported */
                ENGINE_DEBUG    = 2,       /* Most verbose logs, every vulkan message passes through */
            };

        public:
            static inline std::ostream& info = std::cout;  /* These will get replaced whed debug::init gets called */
            static inline std::ostream& warn = std::cerr;
            static inline std::ostream& error = std::cerr;

        public:
            constexpr static std::array<const char*, 1> REQUIRED_VK_LAYERS = {
                "VK_LAYER_KHRONOS_validation"
            };
    
            constexpr static std::array<const char*, 2> REQUIRED_VK_EXTENSIONS = {
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME //TODO: Move to appropriate place
            };
    
    
            static void init(level debug_level, utils::vulkan::instance& instance);

            debug();
            ~debug();

        private:     
            void enable_debug_exts(utils::vulkan::instance& instance);
            void disable_debug_exts(utils::vulkan::instance& instance);

            static bool get_validation_support();
            static vk::Bool32 debug_callback(
                vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                vk::DebugUtilsMessageTypeFlagsEXT type,
                const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
                void* user_data);
                
        private: 
            inline static debug* s_instance = nullptr;   
            
            /* Vulkan state */
            bool                       m_vk_is_setup = false;
            level                      m_dbg_level;        
            vk::DebugUtilsMessengerEXT m_messenger;

    };
}