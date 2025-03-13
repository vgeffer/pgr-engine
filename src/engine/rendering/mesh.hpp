#pragma once
#include "../../lib/glad/glad.h"
#include "material.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <utility>

namespace rendering {


    class mesh {
        public:
            /* Renderer manages drawing */
            virtual ~mesh();
            inline std::pair<GLuint, GLuint> drawable() const { return std::make_pair(draw_mode, attr_buffer); }
            

        protected: 
            explicit mesh(); 

            GLuint draw_mode; /* GL_LINES/GL_STRIP, etc... */
            /* Somehow pass arrays/indexed */
            GLuint attr_buffer;

            GLuint vertex_buffer; /* Interleaved with normals & texels */
            GLuint index_buffer;

            /* TODO: State management made easy: */
            /*
                - Before mesh is created, check if it's occurence counter is greater than 0 
                    - If yes, increase it and retrieve it's data
                    - If no, load it and set counter to 1
                - Mesh is picked up by the renderer. It optimises it based on the material
                    |- Adding it to an instance group
                    \- Creating new rendering_prop and sort it in 
                - When the model is deleted:
                    - Decrease occurence counter - if 0, delete the model
                    - Remove it from instance group
                    - Or delete rendering_prop 
            */

    };
    
    class mesh_instance {
        
        public:
            virtual ~mesh_instance();  
              
            std::shared_ptr<mesh> mesh;
            material material;

            inline mesh_instance* prev() const { return _prev; }
            inline mesh_instance* next() const { return _next; }
        
            void request_draw();

        private:
            mesh_instance *_prev,
                          *_next;

            bool _draw_enqueued = false;
    };


}
