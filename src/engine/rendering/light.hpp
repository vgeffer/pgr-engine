#pragma once

#include "../scene/scene_node.hpp"
#include "../utils/resource.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <tuple>

namespace rendering {
    class light : public scene::node_component {

        public:
            enum class light_type {
                DIRECTIONAL,
                POINT,
                SPOT,
            };

            struct light_data_t {
                light_type type;
            
                glm::vec3 ambient;           
                glm::vec3 diffuse;
                glm::vec3 specular;
            
                glm::vec3 position;
                glm::vec3 direction;
            
                float constant;  /* POINT and SPOT*/
                float linear;    /* POINT and SPOT*/
                float quadratic; /* POINT and SPOT*/
                float angle;     /* SPOT only */
            };

        public:
            /* Color Getters/Setters - available for all types of lights */
            inline glm::vec3 ambient() const { return m_data.ambient; }
            inline glm::vec3 ambient(const glm::vec3& a) { return m_data.ambient = a; } 

            inline glm::vec3 diffuse() const { return m_data.diffuse; }
            inline glm::vec3 diffuse(const glm::vec3& d) { return m_data.diffuse = d; }
 
            inline glm::vec3 specular() const { return m_data.specular; }
            inline glm::vec3 specular(const glm::vec3& s) { return m_data.specular = s; }

            void update();

        protected:
            explicit light(scene::scene_node* parent, light_type type, glm::vec3 ambient, glm::vec3 diffuse, 
                           glm::vec3 specular, float range, float angle);

            light_data_t m_data;
    
        private:
            void prepare_draw(const glm::mat4x4& parent_transform);
    };

    namespace lights {

        class directional_light : public light {

            public:
                directional_light(scene::scene_node* parent);
                directional_light(scene::scene_node* parent, const utils::resource& res);
        };

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