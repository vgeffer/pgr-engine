#pragma once
#include <set>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>
#include <cassert>

#define FIELD(field_type, field)                  \
    public:                                       \
        static inline const field_type& field() noexcept { \
            assert(s_instance != nullptr);        \
            return s_instance->m_##field;         \
        }                                         \
    private:                                      \
        field_type m_##field;

namespace pgreng::app {

    class app_config {
        public:

            app_config() = default;
            void init(std::string path);

            FIELD(std::string, project_name);
            
            FIELD(float, physics_interval);
            FIELD(std::string, default_scene_path);
            FIELD(std::vector<std::string>, default_shaders);

            FIELD(vk::ApplicationInfo, vk_app_info);
            FIELD(std::vector<std::string>, vk_required_extensions);

            FIELD(std::set<std::string>, vk_required_features);

            /* A very hacky hack to get around macro system sucking */
            #define uomap std::unordered_map<std::string, int>
            FIELD(uomap, vk_optional_features);
            #undef uomap
            
        private:
            inline static app_config* s_instance = nullptr;
    };
}
