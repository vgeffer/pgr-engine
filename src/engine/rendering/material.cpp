#include "material.hpp"
#include <array>
#include <exception>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>
#include "renderer.hpp"
#include "../assets/loader.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

using namespace glm;
using namespace rendering;

material::material() 
    : m_material_index(-1), m_uv_mat(glm::identity<glm::mat3x3>()) {

    m_data.ambient =  vec3(1.0f, 0.2f, 0.6f);
    m_data.specular = vec3(0.0f, 0.0f, 0.0f);
    m_data.diffuse =  vec3(0.0f, 0.0f, 0.0f);
    m_data.roughness = 1.0f;
    m_data.metalic = 0.0f;
    m_data.alpha = 0.0f;

    m_data.bound_textures_count = ivec4(0);
    m_fill_empty_shaders();
}

material::material(const utils::resource& res)
    : m_material_index(-1), m_uv_mat(glm::identity<glm::mat3x3>()) {

    using namespace glm;
    using namespace nlohmann;

    std::vector<std::string> used_shaders = res.deserialize<std::vector<std::string>>("shaders");
    for (const auto& shader_path : used_shaders) {

        auto shader = assets::loader::load<assets::shader_stage>(shader_path);

        if (m_shader_stages.find(shader->type_bitmask()) != m_shader_stages.end()) {
            std::cerr << "[WARNING] Skipping shader " << shader_path << " as another shader of same type is already in use!" << std::endl; 
            continue;
        }
        m_shader_stages[shader->type_bitmask()] = shader;
    }
    /* Add shaders to the empty spots */
    m_fill_empty_shaders();

    /* Deserialize internal data structure */
    m_data.ambient = res.deserialize<vec3>("colors/ambient", vec3(0, 1, 1));
    m_data.diffuse = res.deserialize<vec3>("colors/diffuse", vec3(1, 1, 1));
    m_data.specular = res.deserialize<vec3>("colors/specular", vec3(0, 0, 0));
    m_data.roughness = res.deserialize<float>("roughness", 1.0f);
    m_data.metalic = res.deserialize<float>("metalic", 0.0f);
    m_data.alpha = res.deserialize<float>("alpha", 1.0f);

    json diffuse_textures = res.deserialize<json>("textures/diffuse", {});
    for (int i = 0; i < std::min(diffuse_textures.size(), 2ul); i++)
        m_diffuse_textures[i] = assets::loader::load<assets::texture>(diffuse_textures[i]);

    json specular_textures = res.deserialize<json>("textures/specular", {});
    for (int i = 0; i < std::min(specular_textures.size(), 2ul); i++)
        m_specular_textures[i] = assets::loader::load<assets::texture>(specular_textures[i]);

    json normal_maps = res.deserialize<json>("textures/normal", {});
    for (int i = 0; i < std::min(normal_maps.size(), 2ul); i++)
        m_normal_maps[i] = assets::loader::load<assets::texture>(normal_maps[i]);

    json blend_maps = res.deserialize<json>("textures/blend", {});
    for (int i = 0; i < std::min(blend_maps.size(), 2ul); i++)
        m_blend_maps[i] = assets::loader::load<assets::texture>(blend_maps[i]); /* Save texture & assign handle*/


    m_data.bound_textures_count = ivec4(
        std::min(diffuse_textures.size(), 2ul),
        std::min(specular_textures.size(), 2ul),
        std::min(normal_maps.size(), 2ul),
        std::min(blend_maps.size(), 2ul)
    );
}

material::material(glm::vec3 a, glm::vec3 d, glm::vec3 s, float sh, float al) 
    : m_data({a, d, s, sh, al}), m_uv_mat(glm::identity<glm::mat3x3>()) {
}

material::~material() {
    
    if (m_material_index < 0)
        return;

    /* Only do cleanup when material has data to clean up */
    renderer::instance()->material_allocator().free_buffer(m_buffer_handle);
}

void material::use() {

    /* Create defaults for textures */
    m_data.diffuse_texture_ids = { -1, -1 };
    m_data.specular_texture_ids = { -1, -1 };
    m_data.normal_map_ids = { -1, -1 };
    m_data.blend_map_ids = { -1, -1 };

    /* Register all used textures */
    for (int i = 0; i < m_data.bound_textures_count[0]; i++) {
        m_diffuse_textures[i]->use();
        m_data.diffuse_texture_ids[i] = m_diffuse_textures[i]->texture_index();
    }
    
    for (int i = 0; i < m_data.bound_textures_count[1]; i++) {
        m_specular_textures[i]->use();
        m_data.specular_texture_ids[i] = m_specular_textures[i]->texture_index();
    }

    for (int i = 0; i < m_data.bound_textures_count[2]; i++) {
        m_normal_maps[i]->use();
        m_data.normal_map_ids[i] = m_normal_maps[i]->texture_index();
    }

    for (int i = 0; i < m_data.bound_textures_count[3]; i++) {
        m_blend_maps[i]->use();
        m_data.blend_map_ids[i] = m_blend_maps[i]->texture_index();
    }

    /* Register material with renderer */
    auto [handle, offset] = renderer::instance()->material_allocator().alloc_buffer(sizeof(m_data));
    rendering::renderer::instance()->material_allocator().buffer_data(handle, sizeof(m_data), &m_data);

    /* Calculate index */
    m_material_index = offset / sizeof(m_data);
    m_buffer_handle = handle;
}

void material::m_fill_empty_shaders() {

    const auto& default_shaders = renderer::instance()->default_shaders();

    for (GLbitfield type : assets::shader_stage::c_known_stage_types) {
        
        if (m_shader_stages.find(type) != m_shader_stages.end())
            continue;
        
        try { m_shader_stages[type] = default_shaders.at(type); }
        catch(std::exception& e) { throw std::logic_error(
            std::string("An error occured during emplacing default shaders: ") + e.what()
        ); }
    }
}