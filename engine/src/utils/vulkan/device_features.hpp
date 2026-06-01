#pragma once
#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <ostream>
#include <string>

namespace utils::vulkan {

    class device_features {

        public:
            /* TODO: Device feature matrix */
            struct rating {
                enum suitability {
                    NOT_SUITABLE = 0, SUBOPTIMAL = 1, OPTIMAL = 2
                } suitability;
                int score;

                /* For easier logging / Debugging */
                bool is_suitable() const noexcept { return suitability != suitability::NOT_SUITABLE; }

                bool operator<(const rating& other) const noexcept;
                bool operator==(const rating& other) const noexcept;
                bool operator>(const rating& other) const noexcept;

                friend std::ostream& operator<<(std::ostream& out, const rating& r);
            };

            struct feature_wrapper {
                enum requirement : uint8_t {
                    REQUIRED, OPTIONAL, UNUSED
                };

                feature_wrapper();
                feature_wrapper(bool feature);
                feature_wrapper(bool feature, const std::string& name);

                operator bool() const noexcept { return m_feature; }
                const requirement status() const noexcept { return m_status; } 

                private:
                    bool operator=(const bool other) { return m_feature = other; }
                    bool        m_feature;
                    requirement m_status;
                    uint32_t    m_optional_score;

                friend class device_features;  /* So operator= is visible */
            };


            struct limit_wrapper {
                /*enum requirement : uint8_t {
                    REQUIRED, OPTIONAL, DONT_CARE
                };

                feature_wrapper();
                feature_wrapper(bool feature);
                feature_wrapper(bool feature, const std::string& name);

                operator bool() const noexcept { return m_feature; }
                const requirement status() const noexcept { return m_status; } 

                private:
                    bool operator=(const bool other) {m_feature = other; }
                    bool        m_feature;
                    requirement m_status;

                friend class device_features;  /* So operator= is visible */
            };

        public:
            device_features(const vk::PhysicalDevice& dev);
            rating rate_device();
            vk::PhysicalDeviceFeatures get_used_features();

            const std::string& name() const noexcept { return m_dev_name; }
            const vk::PhysicalDeviceType& type() const noexcept { return m_dev_type; }

            feature_wrapper has_graphics_queue;
            feature_wrapper has_presentation_queue;
            feature_wrapper has_compute_queue;
            feature_wrapper has_geometry_shader;
            feature_wrapper has_tessellation_shader;
            feature_wrapper has_dual_src_blend;
            feature_wrapper has_logic_op;
            feature_wrapper has_multidraw_indirect;
            feature_wrapper has_draw_indirect_first_instance;
            feature_wrapper has_depth_clamp;

            
        private:
            std::string                m_dev_name;
            vk::PhysicalDeviceType     m_dev_type;
    };
};