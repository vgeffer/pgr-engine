#pragma once
#include "../assets/shader.hpp"
#include "../nodes/scene_node.hpp"

#include <glm/glm.hpp>
#include <string>

namespace rendering {

    class renderer {

        public:
            renderer();
            static renderer* instance();
             
            void draw_scene(nodes::scene_node* root_node);

            void attach_stage (assets::shader_stage& stage);
            void set_uniform(std::string uniform_name, glm::mat3x3& mat);
            void set_uniform(std::string uniform_name, glm::mat4x4& mat);

        private:
            static renderer* _instance; /* TODO: Guard against multiple inits */
            GLenum _gl_pipeline;

        

    };
}