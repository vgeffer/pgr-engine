#pragma once
#include "vulkan/vulkan.hpp"
#include <vk_mem_alloc.hpp>

namespace pgr::Core::Memory {
    class GpuMemory {
        
        public:
            GpuMemory() {}
            GpuMemory(std::nullptr_t) {}

            GpuMemory(const vk::Instance& instance, const vk::Device& device, const vk::PhysicalDevice physDevice);
            ~GpuMemory();


            GpuMemory(const GpuMemory&) = delete;

            
            GpuMemory& operator=(GpuMemory&& other) noexcept;


        private:
            vma::Allocator mAllocator = nullptr;
    };
}