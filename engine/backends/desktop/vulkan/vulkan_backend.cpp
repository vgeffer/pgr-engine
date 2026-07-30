//#include "instance.hpp"
#include "device.hpp"
#include "device_features.hpp"
#include "vulkan/vulkan.hpp"
//#include "../project_settings.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace utils::vulkan;

/*instance::instance() 
    : m_was_setup(false) {}
    
instance::~instance() {
    if (!m_was_setup)
        return;

    instance_destroyed.invoke(*this);
    m_instance.destroy();
}

void instance::add_extensions(uint32_t count, const char* const* names) {

    assert(!m_was_setup);

    for (uint32_t i = 0; i < count; i++)
        m_extensions.emplace_back(names[i]);
}

void instance::add_extensions(const std::vector<std::string>& names) {

    assert(!m_was_setup);

    for (const auto& name : names)
        m_extensions.emplace_back(name.c_str());
}

void instance::add_layers(uint32_t count, const char* const* names) {

    assert(!m_was_setup);

    for (uint32_t i = 0; i < count; i++)
        m_layers.emplace_back(names[i]);
}

void instance::add_layers(const std::vector<std::string>& names) {

    assert(!m_was_setup);

    for (const auto& name : names)
        m_layers.emplace_back(name.c_str());
}

void instance::create() {

    vk::InstanceCreateInfo instance_info = vk::InstanceCreateInfo()
        .setFlags(vk::InstanceCreateFlags::BitsType::eEnumeratePortabilityKHR)
        .setPApplicationInfo(&project_settings::vk_app_info())
        .setPEnabledExtensionNames(m_extensions)
        .setPEnabledLayerNames(m_layers);

    m_instance = vk::createInstance(instance_info);
    m_was_setup = true;
    instance_created.invoke(*this);
    
    /* Flush extensions and layers buffers 
    m_extensions.clear();
    m_layers.clear();


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