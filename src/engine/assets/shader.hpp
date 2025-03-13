/// 
/// @file shader.h 
/// Contains definition of shader source, stage and pipeline types
/// Defines uniform handlers
///
#pragma once

#include "../../lib/glad/glad.h"
#include "asset.hpp"
#include <string>

namespace assets {

    enum class stage_type : GLenum {
        SOURCE_VERT = GL_VERTEX_SHADER,
        SOURCE_FRAG = GL_FRAGMENT_SHADER,
        SOURCE_GEOM = GL_GEOMETRY_SHADER,
    };
    
    class shader_stage : public asset{
    
        public:
            explicit shader_stage(const std::string path);
            ~shader_stage();
    
            GLint get_uniform_location(std::string uniform);
            inline GLbitfield type_bitmask() const { return _type_bitmask; }
        
        private:
            void _cache_shader();
            GLbitfield _type_bitmask;
            GLenum _program;
    };
}
