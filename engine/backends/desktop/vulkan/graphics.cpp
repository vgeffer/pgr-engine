#include <algorithm>
#include <vector>
#include <GLFW/glfw3.h>
#include <common/debug.hpp>
#include <vulkan/vulkan.hpp>
#include <backends/backend.hpp>

using namespace pgreng::common;

/* Graphics debug callback */
static vk::Bool32 debug_callback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
    
    switch (severity) {

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            debug::log << callback_data->pMessage << "\n";
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            debug::info << callback_data->pMessage << "\n";
            break;
            
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            debug::warn << callback_data->pMessage << std::endl;
            break;
            
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            debug::error << callback_data->pMessage << std::endl;
            break;
    }
    return vk::False;
}



void pgreng::backends::gfx_init() {

    std::vector<const char*> extensions;
    std::vector<const char*> layers;

    /* Window extensions */
    uint32_t glfw_exts_count;
    const char** glfw_exts = 
        glfwGetRequiredInstanceExtensions(&glfw_exts_count);

    extensions.reserve(glfw_exts_count);
    std::copy(glfw_exts, glfw_exts + glfw_exts_count, std::back_inserter(extensions));

    /* Debug extensions  */

    std::vector<vk::LayerProperties> available_layers = vk::enumerateInstanceLayerProperties();
    for (const auto layer : debug::REQUIRED_VK_LAYERS) {
        
        bool layer_found = false;

        for (const auto& layer_props : available_layers) {
            if (strcmp(layer, layer_props.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found)
            return false;
    }

    layers.emplace_back("VK_LAYER_KHRONOS_validation");
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    /* App extensions */


    vk::ApplicationInfo app_info = vk::ApplicationInfo()
        .setApiVersion(VK_API_VERSION_1_2);

    vk::InstanceCreateInfo instance_info = vk::InstanceCreateInfo()
        .setFlags(vk::InstanceCreateFlags::BitsType::eEnumeratePortabilityKHR)
        .setPApplicationInfo(&app_info)
        .setPEnabledExtensionNames(extensions)
        .setPEnabledLayerNames(layers);


        vk::createInstance(instance_info);   


        vk::DebugUtilsMessengerCreateInfoEXT messenger_info = vk::DebugUtilsMessengerCreateInfoEXT()
            .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | 
                                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | 
                                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
            .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)       
            .setPfnUserCallback(&debug_callback)
            .setPUserData(nullptr);

    vk::detail::DispatchLoaderDynamic loader = vk::detail::DispatchLoaderDynamic(static_cast<vk::Instance>(instance), &vkGetInstanceProcAddr);
    m_messenger = instance.createDebugUtilsMessengerEXT(messenger_info, nullptr, loader);
}

void debug::disable_debug_exts(utils::vulkan::instance& instance) {

    vk::detail::DispatchLoaderDynamic loader = vk::detail::DispatchLoaderDynamic(static_cast<vk::Instance>(instance), &vkGetInstanceProcAddr);
    static_cast<vk::Instance>(instance).destroyDebugUtilsMessengerEXT(m_messenger, nullptr, loader);
    
    m_vk_is_setup = false;
} 

