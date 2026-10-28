#include "SwapChain.hpp"
#include <App/Application.hpp>
#include <Core/Rendering/Device.hpp>
#include "vulkan/vulkan.hpp"
#include <algorithm>
#include <cstring>
#include <vector>
#include <iostream>

using namespace pgr::Core::Presentation;
using App = pgr::App::Application;

/* Helper choose functions */
static vk::SurfaceFormatKHR ChooseFormat(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) {
    std::vector<vk::SurfaceFormatKHR> surfaceFormats = device.getSurfaceFormatsKHR(surface);
    
    for (const auto& format : surfaceFormats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) return format;
    }
    
    return surfaceFormats[0];
}

static vk::PresentModeKHR ChoosePresentMode(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface,  vk::PresentModeKHR preferredMode = vk::PresentModeKHR::eFifo) {
    
    std::vector<vk::PresentModeKHR> surfacePresentModes = device.getSurfacePresentModesKHR(surface);         
    for (const auto& mode : surfacePresentModes) {
        if (mode == preferredMode) return mode;
    }

    /* TODO: Logging */
    std::cout << "Present mode " << (uint32_t)preferredMode << " not available. Falling back to FIFO" << std::endl;
    return vk::PresentModeKHR::eFifo;
}

bool SwapChain::IsSupportedByDevice(const Rendering::PhysDevice& dev) {
    
    std::vector<vk::ExtensionProperties> extProps = dev.DeviceHandle.enumerateDeviceExtensionProperties();
    
    for (const auto& ext : extProps) {
        if (strcmp(ext.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) return true;
    }

    return false;
}

SwapChain::SwapChain(const Window& window) {
    const vk::PhysicalDevice physDev = static_cast<const vk::PhysicalDevice&>(::App::UsedDevice());
    const vk::SurfaceCapabilitiesKHR surfaceCapabilities = physDev.getSurfaceCapabilitiesKHR(window);
    
    /* Query surface properties */
    vk::SurfaceFormatKHR surfaceFormat = ChooseFormat(physDev,  window); 
    vk::PresentModeKHR   presentMode   = ChoosePresentMode(physDev, window, vk::PresentModeKHR::eMailbox); /* TODO: Select based on window preferences */

    /* Query window size and clamp if necesseary*/
    const auto winSize = window.Size();
    mImageSize = (vk::Extent2D){ 
        std::clamp(winSize.first, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
        std::clamp(winSize.second, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height)
    };

    /* Set image count and clamp if there is any upper limit */
    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) imageCount = surfaceCapabilities.maxImageCount; 
    
    vk::SwapchainCreateInfoKHR swapchainCreateInfo = vk::SwapchainCreateInfoKHR()
        .setSurface(window)
        .setMinImageCount(imageCount)
        .setImageExtent(mImageSize)
        .setImageFormat(surfaceFormat.format)
        .setImageColorSpace(surfaceFormat.colorSpace)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setPreTransform(surfaceCapabilities.currentTransform)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(presentMode)
        .setClipped(true)
        .setOldSwapchain(nullptr);

    if (::App::UsedDevice().GraphicsQueueFamily() != ::App::UsedDevice().PresentQueueFamily()) {

        std::array<uint32_t, 2> queueFamilies { ::App::UsedDevice().GraphicsQueueFamily(), ::App::UsedDevice().PresentQueueFamily() };
        swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
                           .setQueueFamilyIndices(queueFamilies);
    
    } else swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        
    mVkSwapChain = static_cast<vk::Device>(::App::UsedDevice()).createSwapchainKHR(swapchainCreateInfo);
    mImageFormat = surfaceFormat.format;

    /* Retrieve images */
    std::vector<vk::Image >swapChainImages = static_cast<vk::Device>(::App::UsedDevice()).getSwapchainImagesKHR(mVkSwapChain);

    /* Build Image Views */ //TODO: Maaaaybe split to separate functions?

    for (const auto& image : swapChainImages) {

        vk::ImageViewCreateInfo viewInfo = vk::ImageViewCreateInfo()
            .setImage(image)
            .setFormat(surfaceFormat.format)
            .setViewType(vk::ImageViewType::e2D)
            .setComponents((vk::ComponentMapping) {
                vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, 
                vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity})
            .setSubresourceRange((vk::ImageSubresourceRange) {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

        vk::ImageView view = static_cast<vk::Device>(::App::UsedDevice()).createImageView(viewInfo);
        mSwapChainImageViews.emplace_back(view);
    }

    mRequestImageSemaphore = static_cast<vk::Device>(::App::UsedDevice()).createSemaphore((vk::SemaphoreCreateInfo){});
}

SwapChain::~SwapChain() {

    for (const auto& view : mSwapChainImageViews)
        static_cast<vk::Device>(::App::UsedDevice()).destroyImageView(view);

    static_cast<vk::Device>(::App::UsedDevice()).destroySemaphore(mRequestImageSemaphore);
    static_cast<vk::Device>(::App::UsedDevice()).destroySwapchainKHR(mVkSwapChain);
    mVkSwapChain = nullptr;
}


uint32_t SwapChain::RequestNextImage() {
    const auto a =  static_cast<vk::Device>(::App::UsedDevice()).acquireNextImageKHR(mVkSwapChain, 1ull << 63, mRequestImageSemaphore, nullptr);

    return a.value;
}
