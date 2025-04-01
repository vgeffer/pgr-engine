#pragma once
#include "../assets/shader.hpp"
#include "../nodes/camera.hpp"
#include "light.hpp"
#include "mesh.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace rendering {


    typedef struct {
        GLuint pp_fbo,
               pp_color_buffer,
               pp_depth_buffer;
        bool pp_use_color_tex,
             pp_use_depth_tex;
        std::shared_ptr<assets::shader_stage> pp_shader;
    } postprocess_pass_t;

    class renderer {

        public:
            renderer();
            ~renderer();

            inline static renderer* instance() { return _instance; }

            size_t add_postprocess_pass(assets::shader_stage* shader, bool append);
            void remove_postprocess_pass(size_t index);

            void insert_mesh(mesh_instance* mesh);
            void draw_scene(nodes::camera& camera);

            inline mesh_instance* draw_list() const { return _draw_list; }
            inline mesh_instance* draw_list(mesh_instance* root) { return _draw_list = root; }

            std::vector<std::pair<GLuint, GLint>> attribute_location(std::string name) const;
            std::vector<std::pair<GLuint, GLint>> uniform_location(std::string name) const;


        private:

            void _attach_stage(std::shared_ptr<assets::shader_stage>& stage);

            void _set_uniform(std::string uniform_name, int& i);
            void _set_uniform(std::string uniform_name, glm::mat3x3& mat);
            void _set_uniform(std::string uniform_name, const glm::mat4x4& mat);

            void _set_texture(std::string texture_name, std::shared_ptr<assets::texture> texture);
            void _set_texture(std::string texture_name, GLuint texture) {}

            GLint _attach_stage(GLbitfield type);

            inline static renderer* _instance = nullptr;
            
            /* Model list */
            mesh_instance* _draw_list;

            /* Lights */
            std::vector<light*> _lights;

            /* Textures */
            GLint _max_textures;
            

            /* Shaders */
            GLenum _gl_pipeline;
            std::vector<std::shared_ptr<assets::shader_stage>> _attached_shader_stages;

            /* Postprocessing */
            GLenum _pp_quad_vbo,
                   _pp_quad_vao;

            std::vector<postprocess_pass_t> _pp_passes;
            std::shared_ptr<assets::shader_stage> _pp_vertex_stage;
    };
}