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
            typedef struct {
                glm::vec3 vertex, normal;
                glm::vec2 uv;
            } vertex_t;


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
            virtual ~mesh_instance();  
              
            std::shared_ptr<mesh> drawable;
            material mat;

            inline mesh_instance* prev() const { return _prev; }
            inline mesh_instance* next() const { return _next; }

            inline mesh_instance* next(mesh_instance* next) { return _next = next; }
            inline mesh_instance* prev(mesh_instance* prev) { return _prev = prev; }

            void request_draw(const glm::mat4x4& transform);
            bool draw_enqueued();
            const glm::mat4x4& model_mat() const { return _model_mat; }

            void on_scene_enter() override;

        protected:
            virtual void prepare_draw() {}

        private:
            mesh_instance *_prev, 
                          *_next;

            bool _draw_enqueued = false;
            glm::mat4x4 _model_mat;
    };
}
