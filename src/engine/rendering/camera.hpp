#pragma once
#include "../../lib/glad/glad.h"
#include "../scene/scene_node.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>


namespace rendering {
    class camera : public scene::node_component {
    
        public:
            camera(scene::scene_node* parent, const utils::resource& res);
            camera(scene::scene_node* parent, float fov, float near, float far, bool main);
            ~camera() override;

            glm::mat4x4 view() const;
            glm::mat4x4 projection() const;

            inline glm::vec3 up() const;
            inline glm::vec3 forward() const;

            /* Setting projection parameters also updates stored matrices */
            inline float fov() const { return m_fov; }
            float fov(const float& fov);
            
            inline float near() const { return m_near; }
            float near(const float& near);
            
            inline float far() const { return m_far; }
            float far(const float& far);

            inline GLuint camera_data() const { return m_camera_data; }

            void make_active();
            void look_at(glm::vec3 target);

        private:
            void scene_enter() override;
            void prepare_draw(const glm::mat4x4& parent_matrix) override;

            float m_fov, m_near, m_far;
            bool m_main;
            GLuint m_camera_data;
    };
}