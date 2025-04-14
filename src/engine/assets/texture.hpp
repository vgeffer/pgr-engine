#pragma once
#include "../../lib/glad/glad.h"
#include "../utils/gpu_memory.hpp"
#include "asset.hpp"
#include <glm/glm.hpp>
#include <string>
#include <utility>

namespace assets {
    class texture : public asset {

        public: 

            texture();

            /// @brief Constructor for the texture 
            /// Loads the texture form the file pointed to by and prepares the OpenGL object.
            ///
            /// @param path Filesystem path of the texture
            texture(const std::string path) ; 
            
            /// @brief Destructor for the texture class
            ~texture();    

            /// @brief Makes texture resident in GPU memory and popuales @c m_texture_index
            void use();   

            /// @brief Getter for texture's parameters (size [px], # of color channels)
            inline std::pair<glm::ivec2, int> texture_params() const { return std::make_pair(glm::ivec2(m_w, m_h), m_channels); }
        
            inline GLuint64 texture_handle() const { return m_texture_handle; }

            int texture_index() const { return m_texture_index; } 

        private:

            GLuint m_texture_obj;                           ///< OpenGL texture object
            GLuint64 m_texture_handle;                      ///< Texture's handle in OpenGL memory
            utils::gpu_allocator::handle m_buffer_handle;   ///< Handle to the texture storage in an internal buffer
            GLint m_texture_index;                          ///< Internal index by which the texture could be accessed in shader
            int m_w,                                        ///< Texture's width in px
                m_h,                                        ///< Texture's height in px
                m_channels;                                 ///< Number of texture's color channels 
        };
}