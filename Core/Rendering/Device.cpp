#include "Device.hpp"
#include "Core/Presentation/SwapChain.hpp"
#include "Core/Presentation/Window.hpp"
#include "vulkan/vulkan.hpp"
#include <algorithm>
#include <vector>
#include <App/Application.hpp>
#include <set>

using namespace pgr::Core::Rendering;

Device::Device(PhysDevice device) {
  
    assert(device.IsComplete());
    float queuePriority = 1.0f;

    /* TODO: extensible for more queues */
    std::set<uint32_t> uniqueFamilies = { device.GraphicsQueueFamily.value(), device.PresentQueueFamily.value() };
    std::vector<vk::DeviceQueueCreateInfo> queueInfos;

    for (const auto& familyIndex : uniqueFamilies) {
        vk::DeviceQueueCreateInfo queueInfo = vk::DeviceQueueCreateInfo()
            .setQueueFamilyIndex(familyIndex)
            .setQueueCount(1)
            .setPQueuePriorities(&queuePriority);
        queueInfos.push_back(queueInfo);
    }
    
    std::vector<const char*> devExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    std::vector<const char*> devLayers     { "VK_LAYER_KHRONOS_validation" };
    vk::PhysicalDeviceFeatures devFeatures;

    vk::DeviceCreateInfo devInfo = vk::DeviceCreateInfo()
        .setQueueCreateInfoCount(uniqueFamilies.size())         //TODO: Wrap smarter
        .setPQueueCreateInfos(queueInfos.data())
        .setPEnabledExtensionNames(devExtensions)
        .setPEnabledFeatures(&devFeatures);
        
    mDevice = device.DeviceHandle.createDevice(devInfo);
    mPhysDevice = device.DeviceHandle;

    mGraphicsQueueFamily = device.GraphicsQueueFamily.value();
    mGraphicsQueue = mDevice.getQueue(device.GraphicsQueueFamily.value(), 0);

    mPresentQueueFamily = device.PresentQueueFamily.value();
    mPresentQueue = mDevice.getQueue(device.PresentQueueFamily.value(), 0);
}

std::vector<PhysDevice> Device::EnumerateAvailable(const vk::Instance& instance, const Presentation::Window& window) {
           
    std::vector<PhysDevice> availableDevices;
    std::vector<vk::PhysicalDevice> instanceDevices = instance.enumeratePhysicalDevices();
    
    /* TODO: Move out to separate function */
    for (const auto& vkDev : instanceDevices) {
    
        PhysDevice dev { .DeviceHandle = vkDev, .Score = 0 };

        vk::PhysicalDeviceProperties           props      = vkDev.getProperties();
        vk::PhysicalDeviceFeatures             feats      = vkDev.getFeatures();
        std::vector<vk::ExtensionProperties>   extProps   = vkDev.enumerateDeviceExtensionProperties();
        std::vector<vk::QueueFamilyProperties> queueProps = vkDev.getQueueFamilyProperties();
      
        /* Check for feature support - Graphics Families */
        for (int i = 0; i < queueProps.size(); i++) {

            if (queueProps[i].queueFlags & vk::QueueFlagBits::eGraphics) dev.GraphicsQueueFamily = i;
            if (vkDev.getSurfaceSupportKHR(i, window)) dev.PresentQueueFamily = i;

            if (dev.IsComplete()) break; /* All queues have been assigned, continue */
        }

        /* Query surface properties */
        vk::SurfaceCapabilitiesKHR surfaceCapabilities   = vkDev.getSurfaceCapabilitiesKHR(window);
        std::vector<vk::SurfaceFormatKHR> surfaceFormats = vkDev.getSurfaceFormatsKHR(window);
        std::vector<vk::PresentModeKHR> surfacePresentModes = vkDev.getSurfacePresentModesKHR(window);         

        /* Device filtering: */
        /* 1. Queues */    if (!dev.IsComplete()) continue;
        /* 2. Swapchain */ if (!Presentation::SwapChain::IsSupportedByDevice(dev)) continue;

        /* Device rating: */ 
        /* 1.  Type */  if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)          dev.Score += 10;
                        else if (props.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)   dev.Score += 1; 
        /* 2. Queues */ if (dev.GraphicsQueueFamily.value() == dev.PresentQueueFamily.value()) dev.Score += 2;

        availableDevices.emplace_back(dev);
    }

    auto comparator = [](const PhysDevice& a, const PhysDevice& b) { return a.Score > b.Score; };
    std::sort(availableDevices.begin(), availableDevices.end(), comparator);

    return availableDevices;
}

bool Device::SupportsPresentation(const Presentation::Window& window) const { /* TODO: WTTF TO DO HERE?? */ return false; }

void Device::Destroy() {
    mDevice.destroy();
}
