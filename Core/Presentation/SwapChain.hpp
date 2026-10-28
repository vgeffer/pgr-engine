#pragma once

#include "Common/Common.hpp"
#include "Core/Presentation/Window.hpp"
#include "vulkan/vulkan.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>
#include <Core/Rendering/Device.hpp>

namespace pgr::Core::Presentation {

    class SwapChain {
        public:
            static bool IsSupportedByDevice(const Rendering::PhysDevice& device);

            SwapChain() = default;
            SwapChain(const Window& window);
            ~SwapChain();

            PGR_NODISCARD const inline vk::Extent2D& FrameBufferSize() const noexcept { return mImageSize; }
            PGR_NODISCARD const inline vk::Format& FrameBufferFormat() const noexcept { return mImageFormat; }
            PGR_NODISCARD const inline std::vector<vk::ImageView>& SwapChainImages() const noexcept { return mSwapChainImageViews; }

            PGR_NODISCARD const inline vk::Semaphore& ImageRequestSemaphore() const noexcept { return mRequestImageSemaphore; }

            PGR_NODISCARD inline operator vk::SwapchainKHR() const noexcept { return mVkSwapChain; }

            PGR_NODISCARD inline uint32_t ImageCount() const { return mSwapChainImageViews.size(); }
            uint32_t RequestNextImage();

        private:
            vk::SwapchainKHR mVkSwapChain = nullptr;    
            std::vector<vk::ImageView> mSwapChainImageViews;
            
            vk::Semaphore mRequestImageSemaphore;
            vk::Extent2D mImageSize;
            vk::Format   mImageFormat;
    };
}
