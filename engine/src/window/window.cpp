#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "video_mode.hpp"
#include <glm/glm.hpp>
#include <cstddef>
#include <stdexcept>

using namespace pgreng::window;

window::window(const vk::Instance& instance, const std::string& title, const video_mode& mode)
    : m_props({false, title, mode}), m_cursor_state(cursor_state::cursor_visible), m_win_handle(nullptr) {

    /* Get primary monitor vidmode */
    const GLFWvidmode* glfw_vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());    
    if (glfw_vidmode == nullptr)
            throw std::runtime_error("Unable to retrieve GLFW video mode");
     
    /* Resolve VidMode params */
    glm::ivec2 size = m_props.current_mode.win_mode() == video_mode::window_mode::BORDERLESS ? glm::ivec2(glfw_vidmode->width, glfw_vidmode->height) : 
                                                                                               m_props.current_mode.size();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_win_handle = glfwCreateWindow(
        size.x, size.y,
        title.c_str(),
        m_props.current_mode.win_mode() != video_mode::window_mode::WINDOW ? glfwGetPrimaryMonitor() : NULL,
        nullptr
    );

    if (m_win_handle == NULL)
        throw std::runtime_error("Window not initialized!");
    
    /* Apply callbacks */
    if (m_props.current_mode.vsync())
        glfwSwapInterval(1);


    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, m_win_handle, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("ASDASDASD");

    m_vk_surface = vk::SurfaceKHR(surface);
    glfwShowWindow(m_win_handle);
}

window::~window() {

    if (!m_win_handle)
        return;

    glfwHideWindow(m_win_handle);
    glfwDestroyWindow(m_win_handle);
}

const cursor_state& window::cursor(const cursor_state& state) {
    glfwSetInputMode(m_win_handle, GLFW_CURSOR, static_cast<int>(state));
    return m_cursor_state = state;
}

void window::close() {
    m_props.is_closing = true;
    glfwHideWindow(m_win_handle);
}
