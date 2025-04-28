/// 
/// @file shader.h 
/// Contains definition of shader source, stage and pipeline types
/// Defines uniform handlers
///
#pragma once

#include "../../lib/glad/glad.h"
#include "asset.hpp"
#include <array>
#include <string>

namespace assets {
    class shader_stage : public asset{
        
        public:
            static constexpr std::array<GLbitfield, 2> c_known_stage_types = {
                GL_VERTEX_SHADER_BIT, GL_FRAGMENT_SHADER_BIT
            };
    
        public:
            /// @brief Constructor for the shader_stage class
            /// Loads, processes and compiles the shader. Also finds all used attributes and uniforms
            ///
            /// @param path Filesystem path of the shader
            shader_stage(const std::string path);
            ~shader_stage();
    
            GLint attribute_location(std::string name) const;
            GLint uniform_location(std::string name) const;
            GLint uniform_block_binding(std::string name) const;
            inline GLbitfield type_bitmask() const { return m_type_bitmask; }
        
            explicit operator GLuint() { return m_program; }

        private:
            GLbitfield m_type_bitmask;
            GLuint m_program;
    };
}
