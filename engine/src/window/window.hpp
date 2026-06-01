///
/// @file game_window.hpp
/// @author geffevil
///

#pragma once
#include "key_code.hpp"
#include "video_mode.hpp"
#include "vulkan/vulkan.hpp"
#include <string>


namespace pgreng::window {
    class window {

        public:  
            /// @brief Struct containing the window's properties and data
            struct props {
                bool is_closing;            ///< Flag signaling wether the window should close
                std::string win_title;      ///< Current title of the window
                video_mode current_mode;    ///< Currently used video mode
            };

        public:
            window(const vk::Instance& instance, const std::string& title, const video_mode& mode);
            ~window();

            /// @brief Creates and initializes new GLFW window
            ///
            /// @param title Title of the window
            /// @param mode Video mode to be used by the window
            void create(const std::string& title, const video_mode& mode);
        
            /// @brief Sets cursor state for the window
            /// @param state Cursor state to be used
            /// @see cursor_state
            const cursor_state& cursor(const cursor_state& state);
            const cursor_state& cursor() const { return m_cursor_state; }
        
            /// @brief Sets the is_closing flag in the window properties 
            void close();

            inline const props& window_props() const { return m_props; } 

        private:
            props          m_props;           ///< Window's properties
            GLFWwindow*    m_win_handle;                
            cursor_state   m_cursor_state;    ///< Current cursor state

            vk::SurfaceKHR m_vk_surface;
    };
}