#pragma once
#include "vulkan/vulkan.hpp"

namespace pgr::Core::Memory {
    class GpuMemory {
        
        public:
            GpuMemory() = default;
            GpuMemory(const vk::PhysicalDevice device);
            ~GpuMemory();


        private:
    };
}