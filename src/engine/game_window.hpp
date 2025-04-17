///
/// @file game_window.hpp
/// @author geffevil
///

#pragma once
#include "window/video_mode.hpp"
#include <GLFW/glfw3.h>
#include <string>

class game_window {

    public:  
    struct window_props_t {
        bool is_closing;
        std::string win_title;
        video_mode current_mode;
        GLFWwindow* glfw_handle;
    };

    public:
        game_window();
        ~game_window();

        void create(const std::string& title, video_mode& mode);
        void toggle_block_cursor();  
        void close();

        inline const window_props_t& props() const { return m_props; } 

    private:
        window_props_t m_props;
        void m_apply_default_hints();
};
