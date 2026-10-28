#include <utility>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "Window.hpp"

using namespace pgr::Core::Presentation;

Window::Window(const vk::Instance& instance, const std::string& title, const VideoMode& videoMode) {

    /* Get primary monitor vidmode */
    const GLFWvidmode* glfw_vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());    
    if (glfw_vidmode == nullptr)
        throw std::runtime_error("Unable to retrieve GLFW video mode");

    /* Resolve VidMode params */
    vk::Extent2D size = videoMode.Mode == VideoMode::WindowMode::BORDERLESS ? vk::Extent2D(glfw_vidmode->width, glfw_vidmode->height) : 
                                                                              vk::Extent2D(videoMode.Width, videoMode.Width);
    
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    mWindowHandle = glfwCreateWindow(
        800,//size.x,
        600, //size.y,
        title.c_str(),
        videoMode.Mode != VideoMode::WindowMode::WINDOW ? glfwGetPrimaryMonitor() : NULL,
        nullptr
    );

    if (!mWindowHandle)
        throw std::runtime_error("Window not initialized!");

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, mWindowHandle, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Unable to retireve Vulkan handle!");
    mVkSurface = vk::SurfaceKHR(surface);
}

Window::~Window() {
 
    /* Just assert that proper cleanup was done */
    assert(mVkSurface == nullptr); 
    assert(mWindowHandle == nullptr);
}

void Window::Present() {

    /* TODO: add other things needed to present the window */
    glfwShowWindow(mWindowHandle);
}

void Window::Destroy(const vk::Instance& instance) {

    if (!mWindowHandle) /* Window was not initialized fully */
       return;

    if (mVkSurface) {   /* Check if the initialization didn't fail before surface creation */
        instance.destroySurfaceKHR(mVkSurface);
        mVkSurface = nullptr;
    }
    
    /* Delete window */
    glfwHideWindow(mWindowHandle);
    glfwDestroyWindow(mWindowHandle);
    mWindowHandle = nullptr;
}

std::vector<const char*> Window::RequiredVkExtensions() {
    
    uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);

    if (count == 0 || extensions == nullptr)
        throw std::runtime_error("Unable to retrieve GLFW extensions");

    std::vector<const char*> out;
    for (uint32_t i = 0; i < count; i++)
        out.emplace_back(extensions[i]);

    return out;
}

const cursor_state& Window::Cursor(const cursor_state& state) {
    glfwSetInputMode(mWindowHandle, GLFW_CURSOR, static_cast<int>(state));
    return mCursorState = state;
}

std::pair<uint32_t, uint32_t> Window::Size() const {

    int width, height;
    glfwGetFramebufferSize(mWindowHandle, &width, &height);
    return std::make_pair(width, height);
}