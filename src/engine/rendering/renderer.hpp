#pragma once
#include "../assets/shader.hpp"
#include "camera.hpp"
#include "../utils/gpu_memory.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include <array>
#include <glm/ext/vector_int4.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../utils/group_list.hpp"
#include "../assets/cubemap.hpp"

namespace rendering {

    class renderer {

        public:
            using pp_pass_handle = std::list<std::shared_ptr<assets::shader_stage>>::iterator;
            using shader_map = std::unordered_map<GLbitfield, std::shared_ptr<assets::shader_stage>>;
            using shader_list = std::vector<std::shared_ptr<assets::shader_stage>>;

        public:
            renderer();
            ~renderer();

            void init();
            inline static renderer* instance() { return s_instance; }

            pp_pass_handle add_postprocess_pass(const std::shared_ptr<assets::shader_stage>& shader);
            void remove_postprocess_pass(const pp_pass_handle& index);

            void add_light(const light::light_data_t& light) { m_lights.push_back(light); }
            
            void set_active_camera(const utils::observer_ptr<camera>& camera);
            void set_active_skybox(const std::shared_ptr<assets::cubemap>& skybox);
            bool has_active_camera() const { return m_active_camera.valid(); }

            void request_draw(const utils::observer_ptr<mesh_instance>& mesh, const glm::mat4x4& transform);
            void draw_scene();

            inline utils::gpu_allocator& vertex_allocator() { return m_vertex_buffer; }
            inline utils::gpu_allocator& element_allocator() { return m_element_buffer; }
            inline utils::gpu_allocator& material_allocator() { return m_material_buffer; }
            inline utils::gpu_allocator& texture_allocator() { return m_texture_buffer; }

            inline const shader_map& default_shaders() const { return m_default_shaders; }

            template <typename Tp>
            void set_uniform(std::string uniform_name, const Tp& c, GLbitfield stage_hint = static_cast<GLbitfield>(-1));

        private:
            enum binding_points {
                VERTEX_SSBO = 0,
                OBJECT_SSBO,
                CAMERA_UBO,
                MATERIAL_SSBO,
                TEXTURE_SSBO,
                LIGHTS_SSBO,
            };

            struct draw_request {
                struct draw_command {
                    uint m_element_count;
                    uint m_instance_count;
                    uint m_first_index;
                    int  m_first_vertex;
                    uint m_base_instance;
                } command;

                struct object_data {
                    glm::mat4x4 object;
                    glm::mat4x4 normal;
                    glm::mat3x3 uv;
                    int mat_index;
                } data;

                bool transparent;
                shader_map used_stages;

                draw_request& operator=(const draw_request& other) {
                    command = other.command; 
                    data = other.data; 
                    transparent = other.transparent;
                    used_stages = other.used_stages; 
                    return *this;
                }
            };

            struct render_pass {
                bool transparent;
                uint object_count;
                shader_list shader_delta;
            };

            struct main_fbo {
                GLuint fbo,
                       opaque_target,
                       accum_target,
                       reveal_target,
                       depth_stencil_target;
            };

            struct posptprocess_fbo {
                GLuint fbo,
                       color_target;
            };


        private:
            template <typename T>
            void m_set_uniform_value(GLuint program, GLint location, const T& val);
            void m_attach_stage(const std::shared_ptr<assets::shader_stage>& stage);
            void m_prepare_drawing(std::vector<render_pass>& passes);
            bool m_has_shader_missmatch(const shader_map& a, const shader_map& b);
            void m_end_draw();
            void m_build_fbos(); 
            void m_destroy_fbos();

        private:
            inline static renderer* s_instance = nullptr;

            /* Render targets */
            main_fbo m_default_target;
            std::array<posptprocess_fbo, 2> m_postprocess_targets;

            /* Shaders */
            GLuint m_pipeline;
            shader_map m_attached_shader_stages;
            shader_map m_default_shaders;
            
            /* Object queue */
            group_list<draw_request, GLuint, GLuint> m_enqueued_objects;
            
            /* Programmable vertex pulling buffers */
            GLuint m_models_vao;
            utils::gpu_allocator m_vertex_buffer,
                                 m_element_buffer;

            /* Object data */
            utils::gpu_allocator m_material_buffer,
                                 m_texture_buffer;

            GLuint m_draw_cmd_queue, ///< Indirect command buffer
                   m_object_storage, ///< Per-object data storage
                   m_light_storage;  ///< Per-light data storage

            /* Lights */
            std::vector<light::light_data_t> m_lights;
            
            /* Camera */
            utils::observer_ptr<camera> m_active_camera;

            /* Skybox */
            std::shared_ptr<assets::cubemap> m_current_skybox;

            /* Fog */
            bool m_fog_enabled;
            glm::vec4 m_fog_color;

            /* Postprocessing */
            utils::gpu_allocator::handle m_quad_handle;
            uint m_quad_first_vertex;

            utils::gpu_allocator::handle m_skybox_handle;
            uint m_skybox_first_vertex;

            std::list<std::shared_ptr<assets::shader_stage>> m_postprocess_passes;

            /* Render-specific shaders */
            std::shared_ptr<assets::shader_stage> m_quad_vertex_shader;   
            std::shared_ptr<assets::shader_stage> m_skybox_vertex_shader;
            std::shared_ptr<assets::shader_stage> m_skybox_fragment_shader;
            std::shared_ptr<assets::shader_stage> m_combination_shader;                
    };
}