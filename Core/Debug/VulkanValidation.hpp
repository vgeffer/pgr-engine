#pragma once 

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include <array>
#include <vulkan/vulkan.hpp>

namespace pgr::Debug {

    /* Encompasses validation layers and other assorted stuff */
    class VulkanValidation {

        public:
            static constexpr std::array<const char*, 1> RequiredVulkanLayers     { "VK_LAYER_KHRONOS_validation" };
            static constexpr std::array<const char*, 1> RequiredVulkanExtensions { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };

        public:
            VulkanValidation() = default;
            ~VulkanValidation();

            static bool IsAvailable() noexcept;

            void Init(const vk::Instance& instance);
            void Destroy(const vk::Instance& instance);

        private:
            vk::DebugUtilsMessengerEXT mValidations = nullptr;
    };

}
