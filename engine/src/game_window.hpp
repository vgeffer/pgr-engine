///
/// @file game_window.hpp
/// @author geffevil
///

#pragma once
#include "window/key_code.hpp"
#include "window/video_mode.hpp"
#include <GLFW/glfw3.h>
#include <string>

/// @brief Main engine window class, wrapper of the GLFW functionality
class game_window {

    public:  
        /// @brief Struct containing the window's properties and data
        struct window_props_t {
            bool is_closing;            ///< Flag signaling wether the window should close
            std::string win_title;      ///< Current title of the window
            video_mode current_mode;    ///< Currently used video mode
            GLFWwindow* glfw_handle;    ///< Handle of the underlying GLFW object
        };

    public:
        game_window();
        ~game_window();

        /// @brief Creates and initializes new GLFW window
        ///
        /// @param title Title of the window
        /// @param mode Video mode to be used by the window
        void create(const std::string& title, video_mode& mode);

        
        /// @brief Sets cursor state for the window
        /// @param state Cursor state to be used
        /// @see cursor_state
        const cursor_state& cursor(const cursor_state& state);
        const cursor_state& cursor() const { return m_cursor_state; }
        
        /// @brief Sets the is_closing flag in the window properties 
        void close();

        inline const window_props_t& props() const { return m_props; } 

    private:
        window_props_t m_props;         ///< Window's properties
        cursor_state m_cursor_state;    ///< Current cursor state

        /// @brief Applies some default hints (like OpenGL version) to the window
        void m_apply_default_hints();
};
