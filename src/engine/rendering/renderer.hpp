#pragma once
#include "../assets/shader.hpp"
#include "camera.hpp"
#include "../utils/gpu_memory.hpp"
#include "light.hpp"
#include "mesh.hpp"

#include <future>
#include <glm/ext/vector_int4.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rendering {

    class renderer {

        public:
            renderer();
            ~renderer();

            inline static renderer* instance() { return _instance; }

            size_t add_postprocess_pass(assets::shader_stage* shader, bool append);
            void remove_postprocess_pass(size_t index);

            void add_light(const light::light_data_t& light) { m_lights.push_back(light); }
            
            void set_active_camera(const utils::observer_ptr<camera>& camera);
            bool has_active_camera() const { return m_active_camera.valid(); }

            void request_draw(const utils::observer_ptr<mesh_instance>& mesh, glm::mat4x4 transform);
            void draw_scene();

            inline utils::gpu_allocator& vertex_allocator() { return m_vertex_buffer; }
            inline utils::gpu_allocator& element_allocator() { return m_element_buffer; }
            inline utils::gpu_allocator& material_allocator() { return m_material_buffer; }
            inline utils::gpu_allocator& texture_allocator() { return m_texture_buffer; }

            std::vector<std::pair<GLuint, GLint>> attribute_location(std::string name) const;
            std::vector<std::pair<GLuint, GLint>> uniform_location(std::string name) const;

            template <typename T>
                void set_uniform(std::string uniform_name, const T& c);

        private:
            enum binding_points {
                VERTEX_SSBO = 0,
                OBJECT_SSBO,
                CAMERA_UBO,
                MATERIAL_SSBO,
                TEXTURE_SSBO,
                LIGHTS_SSBO,
                POSTPROCESS_VERTEX_SSBO,
            };
        
            struct draw_command_elements {
                uint m_element_count;
                uint m_instance_count;
                uint m_first_index;
                int  m_first_vertex;
                uint m_base_instance;
            };

            struct object_data {
                glm::mat4x4 m_object;
                glm::mat4x4 m_normal;
                glm::mat3x3 m_uv;
                int m_mat_index;
            };

            struct render_pass {

                /* Object count */
                uint object_count;
                
                std::vector<std::shared_ptr<assets::shader_stage>> shader_delta;
                std::vector<std::pair<draw_command_elements, object_data>> enqueued_objects;
            };

            struct postprocess_pass {
                GLuint m_fbo;
                GLuint m_color_texture;
                bool m_pass_enabled;
            };

        private:
            void _attach_stage(std::shared_ptr<assets::shader_stage>& stage);
            
            template <typename T>
                void _set_uniform_value(GLuint program, GLint location, const T& val);

        private:

            inline static renderer* _instance = nullptr;

            /* Lights */
            std::vector<light::light_data_t> m_lights;

            std::vector<render_pass> m_enqueued_passes; 
            
            /* Shaders */
            GLenum m_pipeline;
            std::unordered_map<GLbitfield, std::shared_ptr<assets::shader_stage>> m_attached_shader_stages;

            std::shared_ptr<assets::texture> m_invalid_texture;

            GLuint m_models_vao;

            /* Programmable vertex pulling buffers */
            utils::gpu_allocator m_vertex_buffer,
                                 m_element_buffer;

            /* Object data */
            utils::gpu_allocator m_material_buffer,
                                 m_texture_buffer;

            GLuint m_draw_cmd_queue, ///< Indirect command buffer
                   m_object_storage, ///< Per-object data storage
                   m_light_storage;  ///< Per-light data storage


            utils::observer_ptr<camera> m_active_camera;

            /* Postprocessing */
            GLuint m_pp_quad_vbo,
                   m_pp_quad_vao;

            GLuint m_depth_texture;
            GLuint m_pass_depth_attachment;

            std::vector<postprocess_pass> m_passes;
            std::shared_ptr<assets::shader_stage> m_postprocess_vertex_shader;      
    };
}