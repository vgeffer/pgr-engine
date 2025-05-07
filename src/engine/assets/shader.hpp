/// 
/// @file shader.hpp
/// @author geffevil
///
#pragma once

#include "../../lib/glad/glad.h"
#include "asset.hpp"
#include <array>
#include <string>

namespace assets {
    class shader_stage : public asset {
        
        public:
            /// @brief Array of (as of now) recognized and supported shader stage typse
            static constexpr std::array<GLbitfield, 2> c_known_stage_types = {
                GL_VERTEX_SHADER_BIT, GL_FRAGMENT_SHADER_BIT
            };
    
        public:
            /// @brief Constructor for the shader_stage class
            /// Loads, processes and compiles the shader
            ///
            /// @param path Filesystem path of the shader
            shader_stage(const std::string path);

            /// @brief Destructor for the shader_stage class
            /// Destroys OpenGL shader objects and cleans used memory
            ~shader_stage();
    
            /// @brief Getter for type bits of the shader
            inline GLbitfield type_bitmask() const { return m_type_bitmask; }
        
            /// @brief Operator allowing conversion to OpenGL object
            /// This operator allows using shader_stage's underlying OpenGL object directly in OpenGL calls
            /// It is made explicit for code clarity sake
            explicit operator GLuint() { return m_program; }

        private:
            GLbitfield m_type_bitmask;  ///< Shader type bitmask
            GLuint m_program;           ///< OpenGL shader program object
    };
}
