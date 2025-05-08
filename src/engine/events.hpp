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
        events();
        ~events();
        
        events(const events&) = delete;
        events(events&&) = delete;
        
        void process_frame();
        void apply_callbacks(game_window& window);
        
        static bool is_key_pressed(key_code key);
        static bool is_key_held_down(key_code key);
        static bool is_key_released(key_code key);
        static bool is_key_held_up(key_code key);

        static bool is_mouse_pressed(mouse_code button);
        static bool is_mouse_held_down(mouse_code button);
        static bool is_mouse_released(mouse_code button);
        static bool is_mouse_held_up(mouse_code button);
        

        static glm::vec2 mouse_pos();
        static glm::vec2 mouse_delta();
    
    private:
        inline static events* s_instance = nullptr;        

        std::array<uint64_t, 1024> m_key_buffer,
                                   m_key_delta_buffer;    

        uint8_t m_mouse_button_buffer,
                m_mouse_button_delta_buffer;
        
        glm::vec2 m_mouse_pos;
        glm::vec2 m_mouse_delta;
};
