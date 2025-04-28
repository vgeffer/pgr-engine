#pragma once
#include "../../lib/glad/glad.h"
#include "material.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>
#include "../scene/scene_node.hpp"
#include "../utils/gpu_memory.hpp"

namespace rendering {


    class mesh {
        public:
            struct vertex{

                constexpr vertex(glm::vec3&& p, glm::vec3&& n, glm::vec3&& t, glm::vec3&& bt, glm::vec2&& uv)
                    : position(p), normal(n), tangent(t), bitangent(bt), uv(uv) {}

                glm::vec3 position, 
                          normal,
                          tangent,
                          bitangent;
                glm::vec2 uv;
            };

            /* Renderer manages drawing */
            virtual ~mesh();
            inline GLuint mode() const { return m_draw_mode; }
            inline GLuint indexed() const { return m_indexed; }
            inline GLuint element_count() const { return m_element_count; }
            inline GLuint first_vertex() const { return m_first_vertex; }
            inline GLuint first_index() const { return m_first_index; }

        protected: 
            explicit mesh(); 

            GLuint m_draw_mode; /* GL_LINES/GL_STRIP, etc... */
            bool m_indexed;
            GLuint m_element_count;

            utils::gpu_allocator::handle m_vert_handle;
            utils::gpu_allocator::handle m_elem_handle;
            GLuint m_first_vertex;
            GLuint m_first_index;

    };
    
    class mesh_instance : public scene::node_component {
        
        public:
            mesh_instance(scene::scene_node* parent, const utils::resource& res);    
            mesh_instance(scene::scene_node* parent, std::shared_ptr<mesh>& drawable);
            ~mesh_instance() override = default;  
              
            std::shared_ptr<mesh>& get_mesh() { return m_mesh; }
            material& get_material() { return m_material; }

        private:
            void scene_enter() override;
            void prepare_draw(const glm::mat4x4& parent_transform) override;

            std::shared_ptr<mesh> m_mesh;
            material m_material;
    };
}
