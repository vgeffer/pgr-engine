#include "../lib/glad/glad.h"
#include "game_window.hpp"
#include "window/video_mode.hpp"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <glm/ext/vector_int2.hpp>
#include <stdexcept>

game_window::game_window()
    : m_props({false, "", video_mode(), nullptr}) {

    /* Initialize GLFW */
    if (!glfwInit())
        throw std::runtime_error("GLFW context initialization failed!");
}

void game_window::create(const std::string& title, video_mode& mode) {

    /* Destroy old window if needed */
    if (m_props.glfw_handle != nullptr) {

        glfwDestroyWindow(m_props.glfw_handle);
    }

    /* Setup new window */
    m_props = game_window::window_props_t{
        false, 
        title, 
        mode,
        nullptr
    };

    /* Get primary monitor vidmode */
    const GLFWvidmode* glfw_vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());    
    if (glfw_vidmode == nullptr)
            throw std::runtime_error("Unable to retrieve GLFW video mode");
     

    /* Resolve VidMode params */
    glm::ivec2 size = m_props.current_mode.win_mode() == video_mode::window_mode::BORDERLESS ? glm::ivec2(glfw_vidmode->width, glfw_vidmode->height) : 
                                                                                               m_props.current_mode.size();
    m_apply_default_hints();


    m_props.glfw_handle = glfwCreateWindow(
        size.x,
        size.y,
        title.c_str(),
        m_props.current_mode.win_mode() != video_mode::window_mode::WINDOW ? glfwGetPrimaryMonitor() : NULL,
        nullptr
    );

    if (m_props.glfw_handle == NULL)
        throw std::runtime_error("Window not initialized!");
    
    /* Apply callbacks */
    if (m_props.current_mode.vsync())
        glfwSwapInterval(1);
    
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_props.glfw_handle));

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("OpenGL context initialization failed!");

    glfwShowWindow(static_cast<GLFWwindow*>(m_props.glfw_handle));
}

game_window::~game_window() {
 
    if (!m_props.glfw_handle)
        return;
    
    glfwHideWindow(m_props.glfw_handle);
    glfwDestroyWindow(m_props.glfw_handle);

    glfwTerminate();
}

void game_window::close() {
    m_props.is_closing = true;
    glfwHideWindow(m_props.glfw_handle);
}

void game_window::m_apply_default_hints() {

    /* Done this way since they are applied in multiple places */
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, static_cast<int>(m_props.current_mode.antialias_level()));
}