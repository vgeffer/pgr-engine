#include "material.hpp"
#include <vector>

using namespace rendering;


material::material(const utils::resource& res)
    : m_material_index(-1) {

    using namespace glm;
    using namespace nlohmann;

    std::vector<std::string> used_shaders = res.deserialize<std::vector<std::string>>("shaders");
    for (const auto& shader_path : used_shaders) {

        auto shader = assets::asset::load<assets::shader_stage>(shader_path);

        if (m_shader_stages.find(shader->type_bitmask()) != m_shader_stages.end()) {
            std::cerr << "Skipping shader " << shader_path << " as another shader of same type is already in use!\n"; 
            continue;
        }
        m_shader_stages[shader->type_bitmask()] = shader;
    }

    /* Deserialize internal data structure */
    m_data.m_ambient = res.deserialize<vec3>("ambient", vec3(0, 0, 0));
    m_data.m_specular = res.deserialize<vec3>("specular", vec3(0, 0, 0));
    m_data.m_diffuse = res.deserialize<vec3>("diffuse", vec3(0, 0, 0));
    m_data.m_shininess = res.deserialize<float>("shininess", 0.0f);
    m_data.m_alpha = res.deserialize<float>("alpha", 0.0f);
    
    json diffuse_textures = res.deserialize<json>("textures/diffuse", {});
    for (int i = 0; i < std::min(diffuse_textures.size(), 2ul); i++)
        m_data.m_diffuse_textures[i] = assets::asset::load<assets::texture>(diffuse_textures[i]);
    
    json specular_textures = res.deserialize<json>("textures/specular", {});
    for (int i = 0; i < std::min(specular_textures.size(), 2ul); i++)
        m_data.m_specular_textures[i] = assets::asset::load<assets::texture>(specular_textures[i]);

    json normal_maps = res.deserialize<json>("textures/normal", {});
    for (int i = 0; i < std::min(normal_maps.size(), 2ul); i++)
        m_data.m_normal_maps[i] = assets::asset::load<assets::texture>(normal_maps[i]);

    json blend_maps = res.deserialize<json>("textures/blend", {});
    for (int i = 0; i < std::min(blend_maps.size(), 2ul); i++)
        m_data.m_blend_maps[i] = assets::asset::load<assets::texture>(blend_maps[i]);

    m_data.m_bound_textures_count = ivec4(
        std::min(diffuse_textures.size(), 2ul),
        std::min(specular_textures.size(), 2ul),
        std::min(normal_maps.size(), 2ul),
        std::min(blend_maps.size(), 2ul)
    );
}

material::material(glm::vec3 a, glm::vec3 d, glm::vec3 s, float sh, float al) 
    : m_data({a, d, s, sh, al}) {
}

material::~material() {
    /* Invalidate material */
}

void material::use() {

    /* Register all used textures */
    for (int i = 0; i < m_data.m_bound_textures_count[0]; i++)
        m_data.m_diffuse_textures[i]->use();
    
    for (int i = 0; i < m_data.m_bound_textures_count[1]; i++)
        m_data.m_specular_textures[i]->use();

    for (int i = 0; i < m_data.m_bound_textures_count[2]; i++)
        m_data.m_normal_maps[i]->use();

    for (int i = 0; i < m_data.m_bound_textures_count[3]; i++)
        m_data.m_blend_maps[i]->use();

    /* Register material with renderer */
}
