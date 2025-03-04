#pragma once
#include "../../lib/glad/glad.h"


namespace rendering {

    class mesh {
        
        public:
            virtual void draw();


        protected: 
            explicit mesh(); 



            GLuint attr_buffer;
            GLuint vertex_buffer, index_buffer;
            GLuint texel_buffer, normal_buffer;
            

    };
};