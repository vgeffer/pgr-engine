#pragma once
#include "game_window.hpp"
#include "window/key_code.hpp"
#include <glm/ext/vector_float2.hpp>

class events {
 
    public:
        events(game_window& win);
        void process_frame();
        

        static bool is_key_pressed(key_code key);
        static bool is_key_held(key_code key);
        static bool is_key_released(key_code key);

        static bool is_mouse_pressed(key_code key);
        static bool is_mouse_held(key_code key);
        static bool is_mouse_released(key_code key);

        static glm::vec2 mouse_pos();
        static glm::vec2 mouse_delta();
    
    private:
        static events* _instance;
        
        glm::vec2 _mouse_pos;
        glm::vec2 _prev_mouse_pos;
};
