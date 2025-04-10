#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <array>
#include <unordered_map>
#include "../assets/shader.hpp"
#include "../assets/texture.hpp"
#include "../utils/resource.hpp"

namespace rendering {

    /// @todo [Mid-Term]: Add support for dynamic materials
    /// @todo [Mid-Term]: Create and implement Copy on Update mechanism
    /// @todo [Long-Term]: Material caching

    class material {

        public:
            material(const utils::resource& res);
            material(glm::vec3 a, glm::vec3 d, glm::vec3 s, float sh, float al);
            ~material();

            void use();

            inline bool transparent() const { return m_data.m_alpha < 0.95f; }
            inline int material_index() const { return m_material_index; }
            inline std::unordered_map<GLbitfield, std::shared_ptr<assets::shader_stage>>& shader_stages() { return m_shader_stages; }

        private:
            struct m_material_data {
                glm::vec3 m_ambient,
                          m_diffuse,
                          m_specular;

                float m_shininess;
                float m_alpha;

                std::array<std::shared_ptr<assets::texture>, 2> m_specular_textures;
                std::array<std::shared_ptr<assets::texture>, 2> m_diffuse_textures;
                std::array<std::shared_ptr<assets::texture>, 2> m_normal_maps;
                std::array<std::shared_ptr<assets::texture>, 2> m_blend_maps;
                
                glm::ivec4 m_bound_textures_count;
            } m_data;

            std::unordered_map<GLbitfield, std::shared_ptr<assets::shader_stage>> m_shader_stages;
            int m_material_index;
        };
}

MAKE_JSON_PARSABLE(rendering::material, [] (const utils::resource& res) {
    return rendering::material(res);
});