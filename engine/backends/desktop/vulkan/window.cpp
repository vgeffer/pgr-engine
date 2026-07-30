#include <common/runtime/memory.hpp>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <cstddef>
#include <stdexcept>

using namespace pgreng::common::runtime;

/* Define all structs before including */
namespace pgreng::backends {

    struct window {

        bool should_close;
        GLFWwindow* win_handle;
    
    };
    
    #define BACKEND_IMPLEMENTATION
    #include <backends/backend/backend_functions.hpp>


    /* Helper methods */
    void* get_param(hwindow& wnd, window_params name) {
        
        return nullptr;
    }

    extern "C" void win_create(hwindow& wnd, const char* name) {

        hwindow hwnd;

        const GLFWvidmode* glfw_vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());    
        if (glfw_vidmode == nullptr)
            throw std::runtime_error("Unable to retrieve GLFW video mode");

        /* Setup Defaults */
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        wnd = hwnd;
    }

    extern "C" void win_present(hwindow& wnd) {

        /* Get primary monitor vidmode */

     
        /* Resolve VidMode params */
        glm::ivec2 size = *(int*)get_param(wnd, window_params::IWIN_MODE) == 0 ? glm::ivec2(glfw_vidmode->width, glfw_vidmode->height) : 
                                                                                               m_props.current_mode.size();


        w->win_handle = glfwCreateWindow(
            size.x, size.y,
            title.c_str(),
            m_props.current_mode.win_mode() != video_mode::window_mode::WINDOW ? glfwGetPrimaryMonitor() : NULL,
            nullptr
        );

        if (w->win_handle == nullptr)
            throw std::runtime_error("Window not initialized!");
    
        /* Apply callbacks */
        //if (m_props.current_mode.vsync())
        //    glfwSwapInterval(1);


    //VkSurfaceKHR surface;
    //if (glfwCreateWindowSurface(instance, m_win_handle, nullptr, &surface) != VK_SUCCESS)
    //    throw std::runtime_error("ASDASDASD");

    //m_vk_surface = vk::SurfaceKHR(surface);
    glfwShowWindow(wnd->win_handle);
    }

    extern "C" void win_set_param(hwindow& wnd, window_params name, void* value) {



    } 

    void win_destroy(hwindow& wnd) {

        if (!wnd->win_handle)
            return;

        glfwHideWindow(wnd->win_handle);
        glfwDestroyWindow(wnd->win_handle);
    }

    bool win_should_close(const hwindow& wnd) {
        return wnd->should_close;
    }



}