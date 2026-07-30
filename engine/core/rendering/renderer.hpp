///
/// @file renderer.hpp
/// @author geffevil
///
#pragma once
#include <array>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "light.hpp"
#include "../utils/gpu_memory.hpp"
#include "../utils/group_list.hpp"

namespace rendering {

    /// @brief Class containing the rendering pipelines and associated data
    ///
    /// Default multi-stage rendering pipeline. Also handles skybox, default camera and 
    /// post-processing passes
    class renderer {

        public:
            /// @brief Constructor
            ///
            /// Constructs the renderer and setups GPU buffers
            renderer();
            ~renderer();

            /// @brief Initializes the renderer
            ///
            /// Initializes the renderer, setting up all the data used for rendering
            /// and sets up the singleton 
            void init();
            inline static renderer* instance() { return s_instance; }

            /// @brief Prepares and hooks a post-process pass to the pipeline
            ///
            /// @param shader Pointer to the shader asset
            /// @returns Handle of the new post-processing pass
//            pp_pass_handle add_postprocess_pass(const std::shared_ptr<assets::shader_stage>& shader);

            /// @brief Disables and removes post-processing pass
            ///
            /// @param handle Handle of the post-processing pass to be removed
  //          void remove_postprocess_pass(const pp_pass_handle& handle);

            /// @brief Prepares a light for rendering
            //void add_light(const light::light_data& light) { m_lights.push_back(light); }
            
            /// @brief Sets camera as active
            ///
            /// Sets up camera to be active, bining it's data bufer to the shaders
            /// @param camera Camera to be set as active
            void set_active_camera(const utils::observer_ptr<camera>& camera);

            /// @brief Sets active skybox
            ///
            /// @param skybox Cubemap resource to be used for skybox
            //void set_active_skybox(const std::shared_ptr<assets::cubemap>& skybox);

            ///bool has_active_camera() const { return m_active_camera.valid(); }

            /// @brief Requests a rendering of a mesh
            ///
            /// Sets up a draw request to be processed during rendering
            /// @param mesh Mesh to be drawn
            /// @param transform Model matrix for the mesh
           // void request_draw(const utils::observer_ptr<mesh_instance>& mesh, const glm::mat4x4& transform);

            /// @brief Draws the scene
            void draw_scene();

            /// @brief Attaches the stage to the renderer's pipeline object
            ///
            /// If a stage of a same type is already attached, @c stage replaces it
            /// @param stage Shader stage to be attached
            //void attach_stage(const std::shared_ptr<assets::shader_stage>& stage);

            /// @brief Sets a uniform value
            ///
            /// Sets an uniform value to all attached shader stages containing the uniform named @c uniform_name 
            /// @param uniform_name Name of the uniform
            /// @param val Value to be set
            /// @param stage_hint Bitmask of type bits. Sets only the uniform of the stages that have their type bit present
            /// @see assets::shader::set_uniform
            //template <typename Tp>
            //void set_uniform(std::string uniform_name, const Tp& val, GLbitfield stage_hint = static_cast<GLbitfield>(-1));

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
//                shader_map used_stages;

                draw_request& operator=(const draw_request& other) {
                    command = other.command; 
                    data = other.data; 
                    transparent = other.transparent;
                    //used_stages = other.used_stages; 
                    return *this;
                }
            };

            struct render_pass {
                bool transparent;
                uint object_count;
               // shader_list shader_delta;
            };


        private:
            void m_prepare_drawing(std::vector<render_pass>& passes);
            //bool m_has_shader_missmatch(const shader_map& a, const shader_map& b);
            void m_end_draw();
            void m_build_fbos(); 
            void m_destroy_fbos();

        private:
            inline static renderer* s_instance = nullptr;

            /* Render targets */
            //main_fbo m_default_target;  ///< Default Framebuffer
            //std::array<posptprocess_fbo, 2> m_postprocess_targets;  ///< Framebuffers for the post-processing

            /* Shaders */
            //GLuint m_pipeline;                      ///< Shader pipeline
            //shader_map m_attached_shader_stages;    ///< Currently attached shaders
           // shader_map m_default_shaders;           ///< Default shaders
            
            /* Object queue */
            //group_list<draw_request, GLuint, GLuint> m_enqueued_objects;    ///< Objects enqueued to be drawn
            
            /* Programmable vertex pulling buffers */
            //GLuint m_models_vao;    ///< Vertex attrib obect of the global vertex buffer
    };
}