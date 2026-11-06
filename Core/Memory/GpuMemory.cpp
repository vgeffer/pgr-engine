#include <vulkan/vulkan.hpp>

#include <iostream>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.hpp>
#include <Common/Common.hpp>

#include "GpuMemory.hpp"


using namespace pgr::Core::Memory;

GpuMemory::GpuMemory(const vk::Instance& instance, const vk::Device& device, const vk::PhysicalDevice physDevice) {

    vma::VulkanFunctions vkFunctions = vma::functionsFromDispatcher();


    vma::AllocatorCreateInfo allocatorInfo = vma::AllocatorCreateInfo()
        .setDevice(device)
        .setInstance(instance)
        .setPhysicalDevice(physDevice)
        .setVulkanApiVersion(VK_API_VERSION_1_4)
        .setPVulkanFunctions(&vkFunctions);

    mAllocator = vma::createAllocator(allocatorInfo);
    std::cout << "CTOR CALLED " << this << std::endl;
}

GpuMemory::~GpuMemory() {
    
    if (mAllocator == nullptr) return;
    std::cout << "Destructor called " << (void*)this << ", Allocator: " << mAllocator << std::endl;
    mAllocator.destroy();
    mAllocator = nullptr;
}

GpuMemory& GpuMemory::operator=(GpuMemory&& other) noexcept {
    
    mAllocator = other.mAllocator;
    other.mAllocator = nullptr;

    return *this;
}