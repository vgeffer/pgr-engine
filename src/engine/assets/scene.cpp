#include "scene.hpp"
#include "../utils/resource.hpp"

using namespace assets;

scene_template::scene_template(const std::string path) 
    : m_scene_res(utils::resource(path)) {}


scene::scene_node* scene_template::instantiate() {
    return m_scene_res.deserialize<scene::scene_node*>("scene/root");
}