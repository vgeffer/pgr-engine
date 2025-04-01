/// 
/// @file shader.h 
/// Contains definition of shader source, stage and pipeline types
/// Defines uniform handlers
///
#pragma once

#include "../../lib/glad/glad.h"
#include "asset.hpp"
#include <string>
#include <unordered_map>

namespace assets {
    
    class shader_stage : public asset{
    
        public:
            /// @brief Constructor for the shader_stage class
            /// Loads, processes and compiles the shader. Also finds all used attributes and uniforms
            ///
            /// @param path Filesystem path of the shader
            shader_stage(const std::string path);
            ~shader_stage();
    
            GLint attribute_location(std::string name) const;
            GLint uniform_location(std::string name) const;
            inline GLbitfield type_bitmask() const { return _type_bitmask; }
        
            operator GLuint() { return _program; }

        private:
        
            std::unordered_map<std::string, GLint> _used_attrib_locations;  ///< Map of used attributes 
            std::unordered_map<std::string, GLint> _used_uniform_locations;

            GLbitfield _type_bitmask;
            GLuint _program;
    };
}
