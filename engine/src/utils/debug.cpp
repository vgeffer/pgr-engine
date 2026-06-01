#include "debug.hpp"
#include "vulkan/instance.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include <array>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.hpp>

using namespace utils;

debug::debug() {
    s_instance = this;
}

/* TODO: Re-engineer to bind to instance */
debug::~debug() {
    /* TODO: Consider using exceptions */

    /* Ensure vk state was not initialised or the instance (and thus vk state) was already destroyed */
    assert(!m_vk_is_setup);
}

void debug::init(debug::level debug_level,  vulkan::instance &instance) {



    /* Setup stack traces and other assorted doodads */   
    if (s_instance == nullptr) {
        std::cerr << "[WARNING] Debugger was not initialised - Debugging is disabled!" << std::endl;
        return;
    }  /* TODO: Maaaybe make so debug can be omitted??? */

    assert(!s_instance->m_vk_is_setup);

    /* Setup validation layers */
    if (!get_validation_support()) {
        if (debug_level < debug::level::PROJECT_DEBUG)
            return; /* Validation layers are not required */
        
        throw std::runtime_error("Not all required validation layers supported!"); 
    }

    
    /* Try to enable validation layers */
    instance.add_layers(debug::REQUIRED_VK_LAYERS.size(), debug::REQUIRED_VK_LAYERS.data());
    instance.add_extensions(debug::REQUIRED_VK_EXTENSIONS.size(), debug::REQUIRED_VK_EXTENSIONS.data());

    /* Enable debug extensions after the instance is created */
    instance.instance_created += TO_HANDLER_CLASS(instance.instance_created, debug::enable_debug_exts, s_instance);
    instance.instance_destroyed += TO_HANDLER_CLASS(instance.instance_destroyed, debug::disable_debug_exts, s_instance);
}

void debug::enable_debug_exts(utils::vulkan::instance& instance) {

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
    m_messenger = static_cast<vk::Instance>(instance).createDebugUtilsMessengerEXT(messenger_info, nullptr, loader);
    m_vk_is_setup = true; 
}

void debug::disable_debug_exts(utils::vulkan::instance& instance) {

    vk::detail::DispatchLoaderDynamic loader = vk::detail::DispatchLoaderDynamic(static_cast<vk::Instance>(instance), &vkGetInstanceProcAddr);
    static_cast<vk::Instance>(instance).destroyDebugUtilsMessengerEXT(m_messenger, nullptr, loader);
    
    m_vk_is_setup = false;
} 

bool debug::get_validation_support() {

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

    return true;
}

vk::Bool32 debug::debug_callback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {


    switch (severity) {

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            std::cerr << "[DEBUG]: ";
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            std::cerr << "[INFO]: ";
            break;
            
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            std::cerr << "[WARNING]: ";
            break;
            
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            std::cerr << "[ERROR]: ";
            break;
            
    }

        std::cerr << callback_data->pMessage << std::endl;
    return vk::False;
}
