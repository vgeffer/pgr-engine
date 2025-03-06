#include "events.hpp"
#include "game_window.hpp"
#include "window/key_code.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/fwd.hpp>
#include <stdexcept>
#include <sys/types.h>
    

using namespace std;

/* Helper directives for prettier code, mainly for static functions */
#define CURRENT_BUFFER (_instance->_current_buffer)
#define LAST_BUFFER (_instance->_current_buffer ^ 1)
#define GET_KEY(buffer, keycode) (_instance->_key_buffer[buffer][(uint16_t)keycode >> 3] & (1 << ((uint16_t)keycode & 0b0111)))
#define GET_BUTTON(buffer, button) (_instance->_mouse_button_buffer[buffer] & (1 << ((uint8_t)button & 0b0111)))

events::events() {
    _instance = this;
}

void events::apply_callbacks(game_window* window) {

    glfwSetMouseButtonCallback(window->props().glfw_handle, [](GLFWwindow* window, int button, int action, int mods) {
        
        if (!_instance)
            throw std::logic_error("Event handler not initialised");
        
        /* Since this app is strictly single-window, info about it can be safely ignored */
        if (action == GLFW_PRESS)
            _instance->_mouse_button_buffer[_instance->_current_buffer] |= (1 << ((uint8_t)button & 0b0111));
        else if (action == GLFW_RELEASE)
            _instance->_mouse_button_buffer[_instance->_current_buffer] &= ~(1 << ((uint8_t)button & 0b0111));
    });

    glfwSetKeyCallback(window->props().glfw_handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

        if (!_instance)
            throw std::logic_error("Event handler not initialised");

        /* Since this app is strictly single-window, info about it can be safely ignored */
        if (action == GLFW_PRESS)
            _instance->_key_buffer[_instance->_current_buffer][((uint16_t)key >> 3)] |= (1 << ((uint8_t)key & 0b0111));
        else if (action == GLFW_RELEASE)
            _instance->_key_buffer[_instance->_current_buffer][((uint16_t)key >> 3)] &= ~(1 << ((uint8_t)key & 0b0111));
    });

    glfwSetCursorPosCallback(window->props().glfw_handle, [](GLFWwindow* window, double x, double y) {
       
        if (!_instance)
            throw std::logic_error("Event handler not initialised");

        /* Since this app is strictly single-window, info about it can be safely ignored */
        _instance->_mouse_pos_buffer[_instance->_current_buffer].x = static_cast<float>(x);
        _instance->_mouse_pos_buffer[_instance->_current_buffer].y = static_cast<float>(y);
    });
}

void events::process_frame() {

    /* Swap buffers */
    _current_buffer = (_current_buffer + 1) % 2; 
    glfwPollEvents();
}

bool events::is_key_pressed(key_code key) {        
        
    if (!_instance)
        throw std::logic_error("Event handler not initialised");

    return GET_KEY(CURRENT_BUFFER, key) && !GET_KEY(LAST_BUFFER, key);
}
    
bool events::is_key_held(key_code key) {
        
    if (!_instance)
        throw std::logic_error("Event handler not initialised");

    return GET_KEY(CURRENT_BUFFER, key) && GET_KEY(LAST_BUFFER, key);
}

bool events::is_key_released(key_code key) {
    
    if (!_instance)
        throw std::logic_error("Event handler not initialised");

    return !GET_KEY(CURRENT_BUFFER, key);
}
    
bool events::is_mouse_pressed(mouse_code button) {

    if (!_instance)
        throw std::logic_error("Event handler not initialised");

    return GET_BUTTON(CURRENT_BUFFER, button) && !GET_BUTTON(LAST_BUFFER, button);
}

bool events::is_mouse_held(mouse_code button) {

    if (!_instance)
        throw std::logic_error("Event handler not initialised");

    return GET_BUTTON(CURRENT_BUFFER, button) && GET_BUTTON(LAST_BUFFER, button);
}

bool events::is_mouse_released(mouse_code button) {

    if (!_instance)
        throw std::logic_error("Event handler not initialised");

    return !GET_BUTTON(CURRENT_BUFFER, button);
}

glm::vec2 events::mouse_pos() {

    if (!_instance)
        throw std::logic_error("Event handler not initialised");

    return _instance->_mouse_pos_buffer[CURRENT_BUFFER];
}

glm::vec2 events::mouse_delta() {

    if (!_instance)
        throw std::logic_error("Event handler not initialised");

    return _instance->_mouse_pos_buffer[CURRENT_BUFFER] - _instance->_mouse_pos_buffer[LAST_BUFFER];
}
