#include "swapchain.hpp"


using namespace utils::vulkan;


void swapchain::init_instance_support(utils::vulkan::instance& instance) {



    /* Setup instance and select*/
    uint32_t glfw_vk_extensions_count;
    const char** glfw_vk_extensions = 
        glfwGetRequiredInstanceExtensions(&glfw_vk_extensions_count);

    instance.add_extensions(glfw_vk_extensions_count, glfw_vk_extensions);
    instance.add_extensions(REQUIRED_VK_EXTENSIONS.size(), REQUIRED_VK_EXTENSIONS.data());
}

swapchain::swapchain(){}