#include "GpuMemory.hpp"
#include "vulkan/vulkan.hpp"
#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace pgr::Core::Memory;

GpuMemory::GpuMemory(const vk::PhysicalDevice device) {

    vk::PhysicalDeviceMemoryProperties memProps = device.getMemoryProperties();

    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {


        int heapIdx = memProps.memoryTypes[i].heapIndex;
        std::cout << "Heap " << heapIdx << ": Size: " << memProps.memoryHeaps[heapIdx].size << " flag bits: " << (uint32_t)memProps.memoryTypes[i].propertyFlags << std::endl;
    }
}

GpuMemory::~GpuMemory() {

}

