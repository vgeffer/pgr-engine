#pragma once
#include "device.hpp"
#include "device_features.hpp"
#include "vulkan/vulkan.hpp"
#include <cassert>
#include <string>
#include <utility>
#include <vector>
#include "../../events/event_provider.hpp"


namespace utils::vulkan {

    class instance { /* TODO: Derive from instance*/

        public:
            EVENT(instance&) instance_created;
            EVENT(instance&) instance_destroyed;

        public:
            instance();
            ~instance();

            void add_extensions(uint32_t count, const char* const* names);
            void add_extensions(const std::vector<std::string>& names);

            void add_layers(uint32_t count, const char* const* names);
            void add_layers(const std::vector<std::string>& names);

            void create();

            
            const std::vector<std::pair<device_features, vk::PhysicalDevice>>& device_list() const noexcept { return m_suitable_devices; }
            utils::vulkan::device select_device(int dev_index);

            operator vk::Instance() const noexcept { assert(m_was_setup); return m_instance; }
            
        private:
            bool                     m_was_setup;
            vk::Instance             m_instance;

            std::vector<const char*> m_extensions;
            std::vector<const char*> m_layers;
        
            std::vector<std::pair<device_features, vk::PhysicalDevice>> m_suitable_devices;
    }; 
};