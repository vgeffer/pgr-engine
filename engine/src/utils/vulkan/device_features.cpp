#include "device_features.hpp"
#include "vulkan/vulkan.hpp"
#include <string>
#include <type_traits>
#include <vector>
#include "../project_settings.hpp"

using namespace std;
using namespace utils::vulkan;


//TODO: If used and found, write to device properties
#define rate_feature(feature)                                          \
    if (feature.status() != feature_wrapper::UNUSED) {              \
        if (feature.status() == feature_wrapper::REQUIRED && !feature) \
            return rating {rating::NOT_SUITABLE, -1};                  \
        else if (feature.status() == feature_wrapper::OPTIONAL) {      \
            if (feature) score += feature.m_optional_score;            \
            else all_optional_features = false;                        \
        }                                                              \
    }



std::ostream& utils::vulkan::operator<<(std::ostream& stream, const device_features::rating& r) {
    stream << "Suitability: ";
    switch (r.suitability) {
        case device_features::rating::suitability::NOT_SUITABLE:
            stream << "Not Suitable";
            break;

        case device_features::rating::suitability::SUBOPTIMAL:
            stream << "Suitable with Missing Features";
            break;

        case device_features::rating::suitability::OPTIMAL:
            stream << "Suitable - Optimal";
            break;
    }

    
    stream << ", Score: " << r.score;
    return stream;
}

/* Smallest should be device with the best score */
bool device_features::rating::operator<(const device_features::rating& other) const noexcept {

    if (other.suitability == suitability)
        return other.score > score;

    return other.suitability > suitability;
}

bool device_features::rating::operator==(const device_features::rating& other) const noexcept {

    return other.suitability == suitability && other.score == score;
}

bool device_features::rating::operator>(const device_features::rating& other) const noexcept {

    if (other.suitability == suitability)
        return other.score < score;

    return other.suitability < suitability;
}

device_features::feature_wrapper::feature_wrapper() 
    : m_feature(false), m_status(UNUSED), m_optional_score(0) {}

device_features::feature_wrapper::feature_wrapper(bool feature)
    : m_feature(feature), m_status(REQUIRED), m_optional_score(0) {}

device_features::feature_wrapper::feature_wrapper(bool feature, const std::string& name) 
    : m_feature(feature), m_status(UNUSED), m_optional_score(0) {

        const auto& required_features = utils::project_settings::vk_required_features();
        const auto& optional_features = utils::project_settings::vk_optional_features();

        /* Try to find string in required_features */
        if (required_features.find(name) != required_features.end()) {
            m_status = REQUIRED;
            return;
        }

        /* If not required, find it in optional */
        std::remove_reference<decltype(optional_features)>::type::const_iterator item;
        if ((item = optional_features.find(name)) != optional_features.end()) {
            m_status = OPTIONAL;
            m_optional_score = item->second;
        }
}

device_features::device_features(const vk::PhysicalDevice& device) {
 
    vk::PhysicalDeviceProperties device_properties = device.getProperties();
    vk::PhysicalDeviceFeatures device_features = device.getFeatures();
    vector<vk::QueueFamilyProperties> device_queue_families = device.getQueueFamilyProperties();

    /* Initialize features */
    m_dev_name = std::string(device_properties.deviceName);
    m_dev_type = device_properties.deviceType;

    /* Always reqired for graphics */
    has_graphics_queue = feature_wrapper(false);
    has_presentation_queue = feature_wrapper(false);

    /* Maybe optional */
    has_compute_queue       = feature_wrapper(false, "gpu_compute");
    has_geometry_shader     = feature_wrapper(device_features.geometryShader, "geometry_shader");
    has_tessellation_shader = feature_wrapper(device_features.tessellationShader, "tessellation_shader");
    has_dual_src_blend      = feature_wrapper(device_features.dualSrcBlend, "dual_src_blend");
    has_multidraw_indirect  = feature_wrapper(device_features.multiDrawIndirect, "multidraw_indirect");

    for (const auto& queue_family : device_queue_families) {
        if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
            has_graphics_queue = true;

        if (queue_family.queueFlags & vk::QueueFlagBits::eCompute)
            has_compute_queue = true;
    }
}   

device_features::rating device_features::rate_device() {

    bool all_optional_features = true;
    int score = 1;

    rate_feature(has_graphics_queue);
    rate_feature(has_compute_queue);
    rate_feature(has_geometry_shader);
    rate_feature(has_tessellation_shader);
    /* TODO: finish - some day*/

    /* Add constant score */
    switch (m_dev_type) {
        case vk::PhysicalDeviceType::eIntegratedGpu:
            score *= 2;
            break;
        case vk::PhysicalDeviceType::eDiscreteGpu:
            score *= 4;
            break;
        default:
            break;
    }

    return rating {
        all_optional_features ? rating::OPTIMAL : rating::SUBOPTIMAL, 
        score
    };
}