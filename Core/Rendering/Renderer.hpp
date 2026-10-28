///
/// @file renderer.hpp
/// @author geffevil
///
#pragma once

#include "App/AppManifest.hpp"
#include "vulkan/vulkan.hpp"
#include <Core/Rendering/Device.hpp>
#include <Core/Presentation/SwapChain.hpp>
#include <Core/Presentation/Window.hpp>
#include <optional>
#include <vector>

namespace pgr::Core::Rendering {

    /* Device shall outlive this object - TODO: Should make base class system to signify this */
    class Renderer {
        public:
          
            Renderer() = default;
            Renderer(const Presentation::Window& target);
            ~Renderer();

            /// @brief Draws the scene
            void RequestFrame();

            void Dispatch();



        private:
            vk::RenderPass mRenderPass;
            Presentation::SwapChain mSwapChain;

            vk::PipelineLayout mLayout;
            vk::Pipeline mPipeline;

            vk::Fence mFrameDrawnFence;
            vk::Semaphore mRenderingDoneSemaphore;

            vk::CommandPool mCommandPool;
            vk::CommandBuffer mCommandBuffer;

            std::vector<vk::Framebuffer> mFrameBuffers; //TODO: Move to Swapchain?
    };
}
