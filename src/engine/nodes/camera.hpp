#pragma once
#include "scene_node.hpp"
#include <glm/glm.hpp>


namespace nodes {
    class camera : public scene_node {
    
        public:
            camera(float fov, float near, float far);

            void look_at(glm::vec3 target);

            glm::mat4x4& get_view();
            glm::mat4x4& get_projection();
            glm::vec3& get_forward();

            glm::vec3 to_screen(glm::vec3& point);
            
            float fov, near, far;

        private:
            glm::vec3 fwd;
            glm::mat4x4 projection;
    };
}