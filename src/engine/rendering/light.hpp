///
/// @file light.hpp
/// @author geffevil
///
#pragma once

#include <tuple>
#include <glm/glm.hpp>
#include "../scene/scene_node.hpp"
#include "../utils/resource.hpp"

namespace rendering {
    /// @brief Node component, a light source in scene
    class light : public scene::node_component {

        public:
            /// @brief Enum of supported light types
            enum class light_type {
                DIRECTIONAL,
                POINT,
                SPOT,
            };

            /// @brief Struct containing light's data to be sent to the shader
            ///
            /// This structure contains data for a @c phong lighting model
            struct light_data {
                light_type type;    ///< Type of the light 
            
                glm::vec3 ambient;  ///< Ambient color of the light          
                glm::vec3 diffuse;  ///< Diffuse color of the light
                glm::vec3 specular; ///< Specular color of the light
            
                glm::vec3 position;     ///< World-space position of the light 
                glm::vec3 direction;    ///< World-space direction of the light
            
                float constant;  ///< attenuation (only used for @c Point and @c Spot light)
                float linear;    ///< attenuation (only used for @c Point and @c Spot light)
                float quadratic; ///< attenuation (only used for @c Point and @c Spot light)
                float angle;     ///< Light angle (only used for @c Spot light )
            };

        public:
            /* Color Getters/Setters - available for all types of lights */

            inline glm::vec3 ambient() const { return m_data.ambient; }
            inline glm::vec3 ambient(const glm::vec3& a) { return m_data.ambient = a; } 

            inline glm::vec3 diffuse() const { return m_data.diffuse; }
            inline glm::vec3 diffuse(const glm::vec3& d) { return m_data.diffuse = d; }
 
            inline glm::vec3 specular() const { return m_data.specular; }
            inline glm::vec3 specular(const glm::vec3& s) { return m_data.specular = s; }

        protected:
            explicit light(scene::scene_node* parent, light_type type, glm::vec3 ambient, glm::vec3 diffuse, 
                           glm::vec3 specular, float range, float angle);

            light_data m_data;
    
        private:
            void prepare_draw(const glm::mat4x4& parent_transform) override;
    };

    namespace lights {

        /// @brief Class containing data fot a directional light
        class directional_light : public light {

            public:
                directional_light(scene::scene_node* parent);
                directional_light(scene::scene_node* parent, const utils::resource& res);
        };


        /// @brief Class containing data fot a point light
        class point_light : public light {

            public:
                point_light(scene::scene_node* parent);
                point_light(scene::scene_node* parent, const utils::resource& res);

                std::tuple<float, float, float> light_attenuation() const { 
                    return std::make_tuple(m_data.constant, m_data.linear, m_data.quadratic); 
                }

                std::tuple<float, float, float> light_attenuation(float c, float l, float q) {
                    return std::make_tuple(m_data.constant = c, m_data.linear = l, m_data.quadratic = q); 
                }
        };

        /// @brief Class containing data fot a spot light
        class spot_light : public light {
            
            public:
                spot_light(scene::scene_node* parent);
                spot_light(scene::scene_node* parent, const utils::resource& res);


                std::tuple<float, float, float> light_attenuation() const { 
                    return std::make_tuple(m_data.constant, m_data.linear, m_data.quadratic); 
                }

                std::tuple<float, float, float> light_attenuation(float c, float l, float q) {
                    return std::make_tuple(m_data.constant = c, m_data.linear = l, m_data.quadratic = q); 
                }

                float angle() const { return m_data.angle; }
                float angle(float a) { return m_data.angle = a; }
        };
    };
};