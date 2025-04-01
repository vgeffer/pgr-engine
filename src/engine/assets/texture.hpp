#pragma once
#include "../../lib/glad/glad.h"
#include "asset.hpp"
#include <glm/glm.hpp>
#include <string>
#include <utility>

namespace assets {
    class texture : public asset {

        public: 

            /// @brief Constructor for the texture 
            /// Loads the texture form the file pointed to by and prepares the OpenGL object.
            ///
            /// @param path Filesystem path of the texture
            texture(const std::string path) ; 
            
            /// @brief Destructor for the texture class
            ~texture();    

            /// @brief Getter for OpenGL texture object
            operator GLuint() const { return _tex_id; }

            /// @brief Getter for texture's parameters (size [px], # of color channels)
            inline std::pair<glm::ivec2, int> tex_params() const { return std::make_pair(glm::ivec2(_w, _h), _channels); }

        private:

            GLuint _tex_id;         ///< OpenGL texture's object
            int _w,                 ///< Texture's width in px
                _h,                 ///< Texture's height in px
                _channels;          ///< Number of texture's color channels 
        };
}