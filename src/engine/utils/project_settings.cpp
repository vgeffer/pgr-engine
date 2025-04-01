#include "project_settings.hpp"
#include "../assets/resource.hpp"
#include <GL/gl.h>
#include <string>

using namespace std;
using namespace utils;
using namespace assets;

project_settings::project_settings(std::string path) {
    _instance = this;
    resource setting_resx = *asset::load<resource>(path, asset::caching_policy::NO_CACHE);

    /* Init EVERYTHING */
    _gl_global_capabilities = setting_resx.deserialize<vector<uint32_t>>("project/gl_capabilities");
    _tex_min_filter = setting_resx.deserialize<int>("project/textures/min_filter");
    _tex_mag_filter = setting_resx.deserialize<int>("project/textures/mag_filter");
    _physics_interval = setting_resx.deserialize<float>("project/physics/update_interval");
}

    
    