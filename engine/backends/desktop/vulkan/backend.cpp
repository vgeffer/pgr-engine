#include <common/os/dynamic_loader.hpp>
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include <vector>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace pgreng::backends {
    
    struct backend {
        void** func_tables;

    };

    #define BACKEND_IMPLEMENTATION
    #include <backends/backend/backend_functions.hpp>
    

    extern "C" void lib_init(hbackend& backend, common::os::dynamic_loader& loader) {

        /* Allocate function tables */

        /* Populate function tables */
        #define FUNCTION_TABLE asd
        #define LOADER loader



        //#define EXPORT_FUNCTION_TABLES
        //#include <backends/backend/backend_functions.hpp>

        /* Init GLFW */
        if (glfwInit() != GLFW_TRUE)
            throw std::runtime_error("GLFW_INIT_ERROR");
    }


    extern "C" void lib_destroy(hbackend& backend) {

        

    }

    extern "C" void lib_get_info(backend_metadata& metadata) {

        metadata = backend_metadata {
            .name = "Vulkan",
            .device_selection = 1,
        };
    }
}

//void instance::create() {


    /* Do device rating 
    std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
    std::multimap<utils::vulkan::device_features::rating, std::pair<device_features, vk::PhysicalDevice>> rating_map;        

    for (const auto& device : devices) {

        device_features features = device_features(device);
        device_features::rating rating = features.rate_device();

        /* Debug print the devices 
        std::cout << "Device: " << features.name() << ", " << rating << std::endl;

        if (rating.is_suitable())
            rating_map.emplace(rating, make_pair(features, device));
    }

    if (rating_map.empty())
        throw std::runtime_error("No available devices found!");

    for (const auto& [rating, device] : rating_map)
        m_suitable_devices.emplace_back(device);
}

utils::vulkan::device instance::select_device(int dev_index) {

    const auto& [features, dev] = m_suitable_devices[dev_index];    


    return device();
}*/