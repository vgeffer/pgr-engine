#pragma once
#include "Core/Memory/GpuMemory.hpp"
#include "Core/Presentation/Window.hpp"
#include "vulkan/vulkan.hpp"
#include <optional>
#include <vulkan/vulkan.hpp>
#include <Common/Common.hpp>

namespace pgr::Core::Rendering {

    struct PhysDevice final { 
        vk::PhysicalDevice DeviceHandle;
        std::optional<uint32_t> GraphicsQueueFamily,
                                PresentQueueFamily;
        uint32_t Score;
    
        inline bool IsComplete() const noexcept { return DeviceHandle && GraphicsQueueFamily.has_value() && PresentQueueFamily.has_value(); }
    };

    class Device final {

        public:
        

            Device() = default;

            static std::vector<PhysDevice> EnumerateAvailable(const vk::Instance& instance, const Presentation::Window& window); 
            static inline Device Select(const PhysDevice& device) { return Device(device); }
            
            inline void WaitToFinish() const { mDevice.waitIdle(); }

            bool SupportsPresentation(const Presentation::Window& window) const;
            void Destroy();

            inline operator vk::Device() const noexcept { return mDevice; }
            inline explicit operator vk::PhysicalDevice() const noexcept { return mPhysDevice; } 

            PGR_NODISCARD inline uint32_t GraphicsQueueFamily() const noexcept { return mGraphicsQueueFamily; }
            PGR_NODISCARD inline uint32_t PresentQueueFamily()  const noexcept { return mPresentQueueFamily; }            

            PGR_NODISCARD  const inline vk::Queue& GraphicsQueue() const noexcept { return mGraphicsQueue; }
            PGR_NODISCARD  const inline vk::Queue& PresentQueue()  const noexcept { return mPresentQueue; }            


        private:
            Device(PhysDevice deice);

        private:
            Memory::GpuMemory  mDeviceAllocator;
            vk::PhysicalDevice mPhysDevice = nullptr;
            vk::Device         mDevice = nullptr;

            uint32_t  mGraphicsQueueFamily;
            vk::Queue mGraphicsQueue = nullptr;

            uint32_t  mPresentQueueFamily;
            vk::Queue mPresentQueue = nullptr;



            /* available swapchain details */
    };
}

