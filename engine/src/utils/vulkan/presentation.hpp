#pragma once
#include "instance.hpp"
#include <GLFW/glfw3.h>
#include <optional>
#include <array>


namespace utils::vulkan {

    class swapchain {

        public:
            static constexpr std::array<const char*, 1> REQUIRED_VK_EXTENSIONS = {

            };

        public:
            swapchain(const GLFWwindow* window, utils::vulkan::instance& instance);
            ~swapchain();

            static void init_instance_support(utils::vulkan::instance& instance);

        private:
            const  m_vk_inst_ref;
    };
}