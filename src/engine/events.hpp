///
/// @file events.hpp
/// @author geffevil
///
#pragma once
#include "game_window.hpp"
#include "window/key_code.hpp"
#include <array>
#include <glm/ext/vector_float2.hpp>

class events {
 
    public:
        explicit events();
        void process_frame();
        void apply_callbacks(game_window& window);
        
        static bool is_key_pressed(key_code key);
        static bool is_key_held(key_code key);
        static bool is_key_released(key_code key);

        static bool is_mouse_pressed(mouse_code button);
        static bool is_mouse_held(mouse_code button);
        static bool is_mouse_released(mouse_code button);

        static glm::vec2 mouse_pos();
        static glm::vec2 mouse_delta();
    
    private:
        inline static events* _instance = nullptr;
        
        uint8_t _current_buffer = 0;

        std::array<std::array<uint8_t, 8192>, 2> _key_buffer;
        std::array<uint8_t, 2> _mouse_button_buffer;
        std::array<glm::vec2, 2> _mouse_pos_buffer;
};
