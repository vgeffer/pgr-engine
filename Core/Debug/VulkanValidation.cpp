#include "VulkanValidation.hpp"
#include "vulkan/vulkan.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

/* Required vulkan functions to be loaded - TODO: Group in common table */
PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance                                 instance,
                                                              const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
                                                              const VkAllocationCallbacks *              pAllocator,
                                                              VkDebugUtilsMessengerEXT *                 pMessenger)
{
    return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks const * pAllocator )
{
    return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_func(vk::DebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
                                                 vk::DebugUtilsMessageTypeFlagsEXT              messageTypes,
                                                 vk::DebugUtilsMessengerCallbackDataEXT const * pCallbackData,
                                                 void * pUserData) {

    /* TODO: move to separate pipeline */
    switch (messageSeverity) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            std::cerr << "[ERROR] \x1b[91m";
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            std::cerr << "[WARNING] \x1b[93m";
            break;
        
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            std::cerr << "\x1b[90m";
            break;

        default:
            std::cerr << "[INFO] \x1b[90m";
            break;
    }
    std::cerr << pCallbackData->pMessage <<  "\x1b[0m" << std::endl;
    return VK_FALSE;
}


using namespace pgr::Debug;


VulkanValidation::~VulkanValidation() {

    /* Validation layers are cleaned before. here logging and other stuff should be taken care of */
    assert(!mValidations);
}

bool VulkanValidation::IsAvailable() noexcept {

    /* Check for the support of the validation layers */
    std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();
    std::vector<vk::LayerProperties>     layers     = vk::enumerateInstanceLayerProperties();

    /* Check layers */
    int layersPresent = 0;
    for (const auto& requiredLayerName : RequiredVulkanLayers) {
        for (const auto& layer : layers) {

            if (strcmp(layer.layerName, requiredLayerName) == 0) { 
                ++layersPresent;
                break;
            }
        }
    }

    /* Check extensions */
    int extensionsPresent = 0;
    for (const auto& requiredExtensionName : RequiredVulkanExtensions) {
        for (const auto& extension : extensions) {

            if (strcmp(extension.extensionName, requiredExtensionName) == 0) { 
                ++extensionsPresent;
                break;
            }
        }
    }

    return layersPresent >= RequiredVulkanLayers.size() && extensionsPresent >= RequiredVulkanExtensions.size();
}

void VulkanValidation::Init(const vk::Instance& instance) {

    /* Load the extension methods */
    pfnVkCreateDebugUtilsMessengerEXT  = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    
    if (!pfnVkCreateDebugUtilsMessengerEXT || !pfnVkDestroyDebugUtilsMessengerEXT) { 
        throw std::runtime_error("Required extension methods were unable to be loaded"); 
    }

    vk::DebugUtilsMessengerCreateInfoEXT info = vk::DebugUtilsMessengerCreateInfoEXT()
        .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
        .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
        .setPfnUserCallback(debug_func)
        .setPUserData(reinterpret_cast<void*>(this));
    
    mValidations = instance.createDebugUtilsMessengerEXT(info);
}   

void VulkanValidation::Destroy(const vk::Instance& instance) {
    if (mValidations)
        instance.destroyDebugUtilsMessengerEXT(mValidations);
    mValidations = nullptr;
}
