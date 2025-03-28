#pragma once
#include "scene_node.hpp"
#include <glm/glm.hpp>


namespace nodes {
    class camera : public scene_node {
    
        public:
            camera(float fov, float near, float far);

            void look_at(glm::vec3 target);
            glm::vec3 to_screen(glm::vec3& point);
            
            
            glm::mat4x4 view() const;
            inline glm::mat4x4 projection() const { return _projection; }
            inline glm::vec3 forward() const { return _fwd; }

            inline float fov() const { return _fov; }
            inline float near() const { return _near; }
            inline float far() const { return _far; }

            float fov(float& fov);
            float near(float& near);
            float far(float& far);

            GLint render_target() const;
            GLint render_target(GLint& target);

        private:



            glm::vec3 _fwd;
            glm::mat4x4 _projection;
            float _fov, _near, _far;
            GLint _render_target;
    };
}