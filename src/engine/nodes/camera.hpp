#pragma once
#include "../../lib/glad/glad.h"
#include "scene_node.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>


namespace nodes {
    class camera : public scene_node {
    
        public:
            camera(float fov, float near, float far);
            ~camera() override;

            void look_at(glm::vec3 target);
            
            glm::mat4x4 view() const;
            glm::mat4x4 projection() const;

            inline glm::vec3 up() const;
            inline glm::vec3 forward() const;

            /* Setting projection parameters also updates stored matrices */
            inline float fov() const { return _fov; }
            inline float fov(float& fov);
            
            inline float near() const { return _near; }
            inline float near(float& near);
            
            inline float far() const { return _far; }
            inline float far(float& far);

            inline GLuint matrix_buffer() const { return _matrix_buffer; }

        private:
            float _fov, _near, _far;
            GLuint _matrix_buffer;
    };
}