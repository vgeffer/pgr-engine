#include "../lib/glad/glad.h"
#include "game_window.hpp"
#include "window/video_mode.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

game_window::game_window(std::string title, video_mode& mode) 
    : _props({.is_closing = false, .win_title = title, .current_mode = mode}) {

    if (!glfwInit())
        throw std::runtime_error("TEST");

    /* Get primary monitor vidmode */
    const GLFWvidmode* glfw_vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());    
    if (glfw_vidmode == NULL)
            throw std::runtime_error("<GLFW-VIDMODE-ERROR>");
        
    /* Resolve VidMode params */
    int width = _props.current_mode.win_mode == window_mode::BORDERLESS ? glfw_vidmode->width : _props.current_mode.w;
    int height = _props.current_mode.win_mode == window_mode::BORDERLESS ? glfw_vidmode->height : _props.current_mode.h;
    _apply_default_hints();

    _props.glfw_handle = glfwCreateWindow(
        width,
        height,
        title.c_str(),
        _props.current_mode.win_mode != window_mode::WINDOW ? glfwGetPrimaryMonitor() : NULL,
        NULL
    );

    if (_props.glfw_handle == NULL)
        throw std::runtime_error("<ERROR->");
    
    /* Apply callbacks */
    if (_props.current_mode.is_vsync)
        glfwSwapInterval(1);
    
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(_props.glfw_handle));
    
    
    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Unable to init GLAD!");
    glfwShowWindow(static_cast<GLFWwindow*>(_props.glfw_handle));
}

game_window::~game_window() {
 
    if (!_props.glfw_handle)
        return;
    
    glfwHideWindow(_props.glfw_handle);
    glfwDestroyWindow(_props.glfw_handle);
    glfwTerminate();
}

void game_window::update_vidmode() {

    if (_props.glfw_handle == NULL) 
        throw std::logic_error("GLFW window not initialised!");
    
    /* Disable VSync */
    glfwSwapInterval(0);

    /* Hide & destroy the window */
    glfwHideWindow(_props.glfw_handle);
    glfwDestroyWindow(_props.glfw_handle);

    /* Re-create window */
    const GLFWvidmode* glfw_vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (glfw_vidmode == NULL)
        throw std::runtime_error("Changing vidmode failed!");
    
    int width = _props.current_mode.win_mode == window_mode::BORDERLESS ? glfw_vidmode->width : _props.current_mode.w;
    int height = _props.current_mode.win_mode == window_mode::BORDERLESS ? glfw_vidmode->height : _props.current_mode.h;
    
    glfwDefaultWindowHints();
    _apply_default_hints();
    _props.glfw_handle = glfwCreateWindow(
            width,
            height,
            _props.win_title.c_str(),
            _props.current_mode.win_mode != window_mode::WINDOW ? glfwGetPrimaryMonitor() : NULL,
            NULL
    );

    if (_props.glfw_handle == NULL)
        throw std::runtime_error("<ERROR-GLFWCREATEWINDOW>");

    /* Set callbacks */
    //applyCallbacks();
    if (_props.current_mode.is_vsync)
        glfwSwapInterval(1);
    
    glfwMakeContextCurrent(_props.glfw_handle);
    glfwShowWindow(_props.glfw_handle);
}

void game_window::close() {
    _props.is_closing = true;
}

void game_window::_apply_default_hints() {

    /* Done this way since they are applied in multiple places */
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, (int)_props.current_mode.antialias);
}