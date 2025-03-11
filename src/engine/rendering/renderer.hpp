#pragma once
#include "../assets/shader.hpp"
#include "../nodes/scene_node.hpp"
#include "../nodes/camera.hpp"
#include "light.hpp"
#include "material.hpp"
#include "mesh.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace rendering {


    typedef struct {
        GLuint pp_fbo,
               pp_color_buffer,
               pp_depth_buffer;
        bool pp_use_color_tex,
             pp_use_depth_tex;
        assets::shader_stage* pp_shader;
    } postprocess_pass_t;

    class renderer {

        public:
            explicit renderer();
            static renderer* instance();

            int add_postprocess_pass(assets::shader_stage* shader, bool append);
            void remove_postprocess_pass(int index);

            void load_scene(nodes::scene_node* node);
            void draw_scene(nodes::camera& camera);


            void attach_stage(assets::shader_stage& stage);

            void set_uniform(std::string uniform_name, int& i);
            void set_uniform(std::string uniform_name, glm::mat3x3& mat);
            void set_uniform(std::string uniform_name, glm::mat4x4& mat);

            void set_texture(std::string texture_name, assets::texture* texture);
            void set_texture(std::string texture_name, GLuint texture);


            GLuint get_location(std::string name, assets::shader_stage* stage);

        private:
            GLuint _get_location(std::string name, assets::shader_stage* stage);

            static renderer* _instance;
            GLenum _gl_pipeline;

            /* Models */
            mesh* _mesh_linklist_head;

            /* Lights */
            std::vector<light*> _lights;

            /* Postprocessing */
            GLenum _pp_quad_vbo,
                   _pp_quad_vao;
            std::vector<postprocess_pass_t> _pp_passes;
            assets::shader_stage* _pp_vertex_stage;
    };
}