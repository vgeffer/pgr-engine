///
/// @file displacement.hpp
/// @author geffevil
///
#pragma once
#include "../../lib/glad/glad.h"
#include "../rendering/mesh.hpp"
#include "asset.hpp"
#include <glm/ext/vector_float3.hpp>
#include <string>

namespace assets {
    
    /// @brief Mesh provider asset, dynamically generating a mesh from a 2D heightmap
    /// @see rendering::mesh
    class displacement : public asset, public rendering::mesh {

        public:
            /// @brief Asset constructor
            /// A requirement of asset. 
            /// 
            /// @param path Filesystem path to the asset
            /// @see assets::asset
            displacement(const std::string& path); 
            
            
            /// @brief Destructor
            /// 
            /// Destroys stored heightmap and calls underlying @c rendering::mesh destructor
            ~displacement() override;        

            /// @brief Retrieves height at position
            ///
            /// Retrieves height at position given by @c pos - linearly interpolates when neceseary
            ///
            /// @param pos 2D position on heightmap
            /// @returns Interpolated height at given position
            float height_at(const glm::vec2& pos) const;
            
            /// @brief Retrieves height at position
            ///
            /// Retrieves height at position given by @c pos - linearly interpolates when neceseary
            /// This overload is mainly for scene nodes, and thus does not require conversion from @c vec3 (ie. position) to @c vec2
            ///
            /// @param pos 3D position on heightmap, only the @c xz components are used
            /// @returns Interpolated height at given position
            float height_at(const glm::vec3& pos) const;

        private:
            int m_w,                ///< Width of the underyling heightmap
                m_h;                ///< Height of the underlying heightmap  
            uint8_t* m_heightmap;   ///< Heightmap data
    };
}