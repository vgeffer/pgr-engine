#pragma once
#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>
#include <cassert>
#include <common/common.hpp>

namespace pgreng::core::app {

    class app_config {
        public:

            app_config() = default;
            void init(std::string path);

            #define FIELD(field, ...)                                   \
                public:                                                 \
                    static inline const __VA_ARGS__& field() noexcept { \
                        assert(s_instance != nullptr);                  \
                        return s_instance->m_##field;                   \
                    }                                                   \
                private:                                                \
                    __VA_ARGS__ m_##field;

            FIELD(project_name, std::string);
            FIELD(backend_name, std::string);

            FIELD(physics_interval, float);
            FIELD(default_scene_path, std::string);

            #undef FIELD

        private:
            inline static app_config* s_instance = nullptr;
    };
}

