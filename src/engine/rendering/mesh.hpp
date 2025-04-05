#pragma once
#include "../../lib/glad/glad.h"
#include "material.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>
#include "../nodes/scene_node.hpp"

namespace rendering {


    class mesh {
        public:
            struct vertex_t{

                vertex_t() {}
                vertex_t(glm::vec3 v, glm::vec3 n, glm::vec2 uv)
                    : vertex(v), normal(n), uv(uv) {}

                glm::vec3 vertex, normal;
                glm::vec2 uv;
            };


            /* Renderer manages drawing */
            virtual ~mesh();
            
            inline GLuint mode() const { return draw_mode; }
            inline GLuint element_count() const { return  el_count; }
            inline GLuint vao() const { return attr_buffer; }             

        protected: 
            explicit mesh(); 

            GLuint draw_mode; /* GL_LINES/GL_STRIP, etc... */
            /* Somehow pass arrays/indexed */
            GLuint el_count;
            
            GLuint attr_buffer;
            GLuint vertex_buffer; /* Interleaved with normals & texels */
            GLuint index_buffer;

    };
    
    class mesh_instance : public nodes::node_component {
        
        public:
            mesh_instance(std::shared_ptr<mesh> d, utils::observer_ptr<material>&& m) : nodes::node_component(), drawable(d), mat(m) {};
            ~mesh_instance() override = default;  
              
            std::shared_ptr<mesh> drawable;
            utils::observer_ptr<material> mat;

            void request_draw(const glm::mat4x4& transform);
            bool draw_enqueued();
            const glm::mat4x4& model_mat() const { return _model_mat; }

            void on_scene_enter() override;

        protected:
            virtual void prepare_draw() {}

        private:
            bool _draw_enqueued = false;
            glm::mat4x4 _model_mat;
    };
}
