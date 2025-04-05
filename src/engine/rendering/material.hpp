#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "../assets/shader.hpp"
#include "../assets/texture.hpp"

namespace rendering {
    class material {

        public:
            
            inline bool transparent() { return _alpha < 1.0f; }
            inline std::vector<std::shared_ptr<assets::shader_stage>>& shader_stages() { return _shader_stages; }

            void use();

        private:
            std::vector<std::shared_ptr<assets::shader_stage>> _shader_stages;

            glm::vec3 _ambient,
                      _diffuse,
                      _specular;

            float _shininess;
            float _alpha;

            /* TODO: multi-texturing */
            std::shared_ptr<assets::texture> _diffuse_texture,
                                             _specular_texture,
                                             _normal_texture;                               
    };
}
