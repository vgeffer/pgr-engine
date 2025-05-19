///
/// @file material.hpp
/// @author geffevil
///
#pragma once

#include <array>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include "../assets/shader.hpp"
#include "../assets/texture.hpp"
#include "../utils/resource.hpp"

namespace rendering {

    /// @todo [Mid-Term]: Add support for dynamic materials
    /// @todo [Long-Term]: Create and implement Copy on Update mechanism
    /// @todo [Long-Term]: Material caching

    /// @brief Class containing material for rendering
    ///
    /// This class contains data for material used for phong lighting model
    class material {

        public:
            /// @brief Default constructor
            ///
            /// Constructs valid material object with an invalid material using default shaders
            material();

            /// @brief Resource constructor
            ///
            /// Constructor used for loading material from file/scene
            material(const utils::resource& res);

            /// @brief Constructor
            ///
            /// Constructs material from provided colors/parameters
            material(glm::vec3 a, glm::vec3 d, glm::vec3 s, float sh, float al);
            ~material();

            const glm::mat3x3& uv_mat() const { return m_uv_mat; }
            const glm::mat3x3& uv_mat(const glm::mat3x3& other) { return m_uv_mat = other; }

            /// @brief Set up material for use
            ///
            /// Uploads material data to the GPU. It also prepares all the used textures
            void use();

            inline bool transparent() const { return m_data.alpha < 0.95f; }
            inline int material_index() const { return m_material_index; }
            inline std::unordered_map<GLbitfield, std::shared_ptr<assets::shader_stage>>& shader_stages() { return m_shader_stages; }

        private:
            /// @brief Structure containing material data
            struct material_data {
                glm::vec3 ambient,  ///< Ambient color
                          diffuse,  ///< Diffuse color
                          specular; ///< Specular color

                float metalic;      ///< Metalic property of the material (how reflective it is)
                float roughness;    ///< Roughness property of the material (strength of the specular component)
                float alpha;        ///< Alpha component of the material

                std::array<int, 2> diffuse_texture_ids;     ///< Texture ID's of the diffuse textures
                std::array<int, 2> specular_texture_ids;    ///< Texture ID's of the specular textures
                std::array<int, 2> normal_map_ids;          ///< Texture ID's of the normal maps
                std::array<int, 2> blend_map_ids;           ///< Texture ID's of the blend maps

                glm::ivec4 bound_textures_count;            ///< Number of bound textures
            };
        
        private:
            /// @brief Utility to fill unused shaders with defaults
            void m_fill_empty_shaders();
    
        private:
            material_data m_data;
            glm::mat3x3 m_uv_mat;

            std::array<std::shared_ptr<assets::texture>, 2> m_specular_textures;
            std::array<std::shared_ptr<assets::texture>, 2> m_diffuse_textures;
            std::array<std::shared_ptr<assets::texture>, 2> m_normal_maps;
            std::array<std::shared_ptr<assets::texture>, 2> m_blend_maps;

            std::unordered_map<GLbitfield, std::shared_ptr<assets::shader_stage>> m_shader_stages;
            utils::gpu_allocator::handle m_buffer_handle;
            int m_material_index;
        };
}

MAKE_JSON_PARSABLE(rendering::material, [] (const utils::resource& res) {
    return rendering::material(res);
});
