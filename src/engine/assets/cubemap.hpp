///
/// @file cubemap.hpp
/// @author geffevil
///
#pragma once
#include "../../lib/glad/glad.h"
#include "asset.hpp"
#include <glm/glm.hpp>
#include <string>
#include <utility>

namespace assets {

    /// @brief Cubemap asset, used for skybox and reflections
    class cubemap : public asset {

        public: 
            /// @brief Default constructor
            /// Constructs a valid cubemap object in an invalid state
            cubemap();

            /// @brief Asset constructor
            /// A requirement of asset. 
            /// 
            /// @param path Filesystem path to the asset
            /// @see assets::asset
            cubemap(const std::string& path); 
            ~cubemap();    

            /// @brief Getter for the texture properties
            /// @returns Pair of texture's size (in px) and number of channels in the texture file
            inline std::pair<glm::ivec2, int> texture_params() const { return std::make_pair(glm::ivec2(m_w, m_h), m_channels); }

            /// @brief Getter for the OpenGL texture object
            GLuint cubemap_object () const { return m_cubemap_obj; } 

        private:

            GLuint m_cubemap_obj;   ///< OpenGL texture object for the cubemap                   
            int m_w,                ///< Single texture's width
                m_h,                ///< Single texture's height                        
                m_channels;         ///< Number of channels in a single texture                       
    };
}