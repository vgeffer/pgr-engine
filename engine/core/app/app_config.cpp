#include "app_config.hpp"
#include <common/resource.hpp>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>


#define PARSE_NUMERIC_SIZE(field, path)                          \
    string str_##field = setting_resx.deserialize<string>(path); \
    field = strtoul(str_##field.c_str(), nullptr, 10);           \
    if (!isdigit(str_##field.back()))                            \
        field *= UNIT_MAP.at(str_##field.back());

using namespace std;
using namespace utils;
using namespace pgreng::core::app;
using namespace pgreng::common;


void app_config::init(std::string path) {

    resource setting_resx = resource();

    /* Init EVERYTHING */
    m_project_name = setting_resx.get<string>("project/name");
    //m_physics_interval = setting_resx.deserialize<float>("project/physics/update_interval");
    //m_default_scene_path = setting_resx.deserialize<string>("project/game/default_scene");

    /*
    m_vk_app_info = vk::ApplicationInfo()
                        .setPApplicationName(m_project_name.c_str())
                        .setPEngineName("pgrengine")
                        .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
                        .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
                        .setApiVersion(VK_API_VERSION_1_1);
    m_vk_required_extensions = setting_resx.deserialize<vector<string>>("project/vk/required_extensions");
    m_vk_required_features = setting_resx.deserialize<set<string>>("project/vk/required_features");
    m_vk_optional_features = setting_resx.deserialize<unordered_map<string, int>>("project/vk/optional_features");
    */
}
