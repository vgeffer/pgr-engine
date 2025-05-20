#include "project_settings.hpp"
#include "../utils/resource.hpp"
#include <cctype>
#include <string>
#include <unordered_map>


#define PARSE_NUMERIC_SIZE(field, path)                          \
    string str_##field = setting_resx.deserialize<string>(path); \
    field = strtoul(str_##field.c_str(), nullptr, 10);           \
    if (!isdigit(str_##field.back()))                            \
        field *= UNIT_MAP.at(str_##field.back());


using namespace utils;

const std::unordered_map<char, uint> UNIT_MAP = {
    { 'k', 1024 },
    { 'M', 1024 * 1024 },
    { 'G', 1024 * 1024 * 1024 }
};

project_settings::project_settings() {

    s_instance = nullptr;
}

void project_settings::init(std::string path) {
    using namespace std;

    s_instance = this;
    resource setting_resx = resource(path);

    /* Init EVERYTHING */
    m_project_name = setting_resx.deserialize<std::string>("project/name");
    m_gl_global_capabilities = setting_resx.deserialize<vector<uint32_t>>("project/ogl/gl_capabilities");
    m_tex_min_filter = setting_resx.deserialize<int>("project/textures/min_filter");
    m_tex_mag_filter = setting_resx.deserialize<int>("project/textures/mag_filter");
    m_physics_interval = setting_resx.deserialize<float>("project/physics/update_interval");
    m_default_scene_path = setting_resx.deserialize<std::string>("project/game/default_scene");
    m_default_shaders = setting_resx.deserialize<vector<string>>("project/game/default_shaders");

    PARSE_NUMERIC_SIZE(m_gpu_geometry_buffer_alloc_size, "project/ogl/gpu_geometry_buffer_alloc_size")
    PARSE_NUMERIC_SIZE(m_gpu_material_buffer_alloc_size, "project/ogl/gpu_material_buffer_alloc_size")
    PARSE_NUMERIC_SIZE(m_gpu_textures_buffer_alloc_size, "project/ogl/gpu_textures_buffer_alloc_size")
}
