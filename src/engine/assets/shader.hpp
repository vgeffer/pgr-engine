/// 
/// @file shader.h 
/// Contains definition of shader source, stage and pipeline types
/// Defines uniform handlers
///
#pragma once

#include "../../lib/glad/glad.h"
#include "asset.hpp"
#include <string>

enum class stage_type : GLenum {
    SOURCE_VERT = GL_VERTEX_SHADER,
    SOURCE_FRAG = GL_FRAGMENT_SHADER,
    SOURCE_GEOM = GL_GEOMETRY_SHADER,
};

namespace assets {
    class shader_stage : public asset{
    
        public:
            explicit shader_stage(std::string path);
            ~shader_stage();
    
        private:
            stage_type type;
            GLenum program;
    };
}
