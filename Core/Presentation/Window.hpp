///
/// @file game_window.hpp
/// @author geffevil
///

#pragma once
#include "Common/Common.hpp"
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <Core/Presentation/KeyCode.hpp>

namespace pgr::Core::Presentation {
    struct VideoMode {
    
        enum class WindowMode { WINDOW, BORDERLESS, FULLSCREEN };
            
        constexpr inline VideoMode& setWidth(uint32_t width) noexcept { Width = width; return *this; }
        constexpr inline VideoMode& setHeight(uint32_t height) noexcept { Height = height; return *this; }
        constexpr inline VideoMode& setMode(WindowMode mode) noexcept { Mode = mode; return *this; }

        uint32_t Width, Height;
        WindowMode Mode;
    };
    
    class Window {
        public:
            Window() = default;
            Window(const vk::Instance& instance, const std::string& title, const VideoMode& mode);
            ~Window();

            void Present();
            void Destroy(const vk::Instance& instance);
            
            operator vk::SurfaceKHR() const noexcept { return mVkSurface; }    
            
            static std::vector<const char*> RequiredVkExtensions();
            const cursor_state& Cursor(const cursor_state& state);
            const cursor_state& Cursor() const { return mCursorState; } 
            inline bool IsClosing() const { return glfwWindowShouldClose(mWindowHandle); } 
            void swap();

            PGR_NODISCARD std::pair<uint32_t, uint32_t> Size() const;

        private:
            GLFWwindow*     mWindowHandle = nullptr;   ///< Handle of the underlying GLFW object                           
            vk::SurfaceKHR  mVkSurface = nullptr;
            cursor_state    mCursorState; ///< Current cursor state
    };
}
