///
/// @file game_window.hpp
/// @author geffevil
///

#pragma once
#include "window/video_mode.hpp"
#include <GLFW/glfw3.h>
#include <string>

typedef struct {

    bool is_closing;
    std::string win_title;
    video_mode* current_mode;
    GLFWwindow* glfw_handle;

} win_props_t;

class game_window {

    public:  
        game_window(std::string title, video_mode* mode);
        ~game_window();
        void update_vidmode();
        void toggle_block_cursor();  
        void close();

        win_props_t& props();

    private:
        win_props_t _props;
        void _apply_default_hints();
};
