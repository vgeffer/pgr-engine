#include "events.hpp"
#include "game_window.hpp"
#include "window/key_code.hpp"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <cstdint>
#include <iostream>
#include <stdexcept>
    
using namespace std;


events::events() 
    : m_mouse_button_buffer(0), m_mouse_pos(0), m_mouse_delta(0) {
    s_instance = this;
}

events::~events() {
    s_instance = nullptr;
}

void events::apply_callbacks(const game_window& window) {

    std::cerr << "Callbacks attached! (" << &window << ")" << std::endl;

    /* Get initial mouse position - so no weirdness occurs */
    double initial_mouse_x, initial_mouse_y;
    glfwGetCursorPos(window.props().glfw_handle, &initial_mouse_x, &initial_mouse_y);
    m_mouse_pos = glm::vec2(initial_mouse_x, initial_mouse_y);

    /* Since this app is strictly single-window, info about it can be safely ignored */
    glfwSetMouseButtonCallback(window.props().glfw_handle, [](GLFWwindow*, int button, int action, int mods) {
        
        if (!s_instance)
            throw std::logic_error("Event handler not initialised");
        
        uint8_t shift = static_cast<uint8_t>(button);
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            s_instance->m_mouse_button_delta_buffer |= (1 << shift);
    });

    glfwSetKeyCallback(window.props().glfw_handle, [](GLFWwindow*, int key, int scancode, int action, int mods) {

        if (!s_instance)
            throw std::logic_error("Event handler not initialised");

        uint8_t shift = (static_cast<uint16_t>(key) & 0x3F); /* Mask last 6 bits */
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            s_instance->m_key_delta_buffer[static_cast<uint16_t>(key) >> 6] |= (1 << shift);
    });

    glfwSetCursorPosCallback(window.props().glfw_handle, [](GLFWwindow*, double x, double y) {
       
        if (!s_instance)
            throw std::logic_error("Event handler not initialised");

        glm::vec2 new_pos = glm::vec2(x, y);
        s_instance->m_mouse_delta = new_pos - s_instance->m_mouse_pos;
        s_instance->m_mouse_pos = new_pos;
    });
}

void events::process_frame() {
    
    /* Update state for all keys */
    for (int i = 0; i < m_key_buffer.size(); i++) {
        m_key_buffer[i] ^= m_key_delta_buffer[i];   /* Transfer delta buffer to main key buffer */
        m_key_delta_buffer[i] = 0;                  /* Zero out delta buffer */
    }

    /* Update state for all the mouse buttons */
    m_mouse_button_buffer ^= m_mouse_button_delta_buffer;
    m_mouse_button_delta_buffer = 0;

    m_mouse_delta = glm::vec2(0);

    glfwPollEvents();
}

bool events::is_key_pressed(key_code key) {        
        
    if (!s_instance)
        throw std::logic_error("Event handler not initialised");

    uint64_t mask = 1ull << (static_cast<uint16_t>(key) & 0x3F);
    
    return !((s_instance->m_key_buffer[static_cast<uint16_t>(key) >> 6]) & mask) &&
           ((s_instance->m_key_delta_buffer[static_cast<uint16_t>(key) >> 6]) & mask); 
}
    
bool events::is_key_held_down(key_code key) {
        
    if (!s_instance)
        throw std::logic_error("Event handler not initialised");

    uint64_t mask = 1ull << (static_cast<uint16_t>(key) & 0x3F);

    return ((s_instance->m_key_buffer[static_cast<uint16_t>(key) >> 6]) & mask) &&
           !((s_instance->m_key_delta_buffer[static_cast<uint16_t>(key) >> 6]) & mask);
}

bool events::is_key_released(key_code key) {
    
    if (!s_instance)
        throw std::logic_error("Event handler not initialised");

    uint64_t mask = 1ull << (static_cast<uint16_t>(key) & 0x3F);

    return ((s_instance->m_key_buffer[static_cast<uint16_t>(key) >> 6]) & mask) &&
           ((s_instance->m_key_delta_buffer[static_cast<uint16_t>(key) >> 6]) & mask); 
}

bool events::is_key_held_up(key_code key) {
        
    if (!s_instance)
        throw std::logic_error("Event handler not initialised");

    uint64_t mask = 1ull << (static_cast<uint16_t>(key) & 0x3F);

    return !((s_instance->m_key_buffer[static_cast<uint16_t>(key) >> 6]) & mask) &&
           !((s_instance->m_key_delta_buffer[static_cast<uint16_t>(key) >> 6]) & mask); 
}
 
bool events::is_mouse_pressed(mouse_code button) {        
        
    if (!s_instance)
        throw std::logic_error("Event handler not initialised");

    uint8_t mask = 1 << (static_cast<uint8_t>(button) & 0x07);
    
    return !(s_instance->m_mouse_button_buffer & mask) &&
           (s_instance->m_mouse_button_delta_buffer & mask); 
}
    
bool events::is_mouse_held_down(mouse_code button) {
        
    if (!s_instance)
        throw std::logic_error("Event handler not initialised");

    uint8_t mask = 1 << (static_cast<uint8_t>(button) & 0x07);

    return (s_instance->m_mouse_button_buffer & mask) &&
          !(s_instance->m_mouse_button_delta_buffer & mask); 

}

bool events::is_mouse_released(mouse_code button) {
    
    if (!s_instance)
        throw std::logic_error("Event handler not initialised");

    uint8_t mask = 1 << (static_cast<uint8_t>(button) & 0x07);
    
    return !(s_instance->m_mouse_button_buffer & mask) &&
           (s_instance->m_mouse_button_delta_buffer & mask); 
}

bool events::is_mouse_held_up(mouse_code button) {
        
    if (!s_instance)
        throw std::logic_error("Event handler not initialised");

    uint8_t mask = 1 << (static_cast<uint8_t>(button) & 0x07);

    return !(s_instance->m_mouse_button_buffer & mask) && 
           !(s_instance->m_mouse_button_delta_buffer & mask);
}

glm::vec2 events::mouse_pos() {

    if (!s_instance)
        throw std::logic_error("Event handler not initialised");

    return s_instance->m_mouse_pos;
}

glm::vec2 events::mouse_delta() {

    if (!s_instance)
        throw std::logic_error("Event handler not initialised");

    return s_instance->m_mouse_delta;
}
