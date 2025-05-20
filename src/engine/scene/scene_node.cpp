#include "scene_node.hpp"
#include <glm/detail/type_quat.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

using namespace std;
using namespace glm;
using namespace scene;
using namespace utils;
using namespace nlohmann;

scene_node::scene_node(const string& name)
    : scene_node(name, scene_node::node_type::GENERIC) {}

scene_node::scene_node(const string& name, scene_node::node_type type)
    : position(vec3(0,0,0)), scale(vec3(1,1,1)), rotation(quat(0,0,0,0)), 
      m_name(name), m_parent(nullptr), m_enabled(true), m_visible(true), m_in_scene(type == node_type::ROOT), m_type(type) {}

scene_node::scene_node(const resource& res)
    : m_name(res.deserialize<std::string>("name")), m_parent(nullptr), m_enabled(true), m_visible(true), 
      m_in_scene(false), m_type(scene_node::node_type::GENERIC) {

    position = res.deserialize<vec3>("position", vec3(0, 0, 0));
    rotation = res.deserialize<quat>("rotation", quat(0, 0, 0, 0));
    scale = res.deserialize<vec3>("scale", vec3(1, 1, 1));

    /* Parse out node components */
    json component_map = res.deserialize<json>("components", {});
    for (auto [name, component] : component_map.items()) {

        auto spawner = _internal::component_registry::registered_components.find(name);
        if (spawner == _internal::component_registry::registered_components.end()) {
            std::cerr << "[WARNING] Unknown component " << name << "! Skipping..." << std::endl;     
            continue;
        }

        /* Call the spawner fn */
        spawner->second(this, component);
    }

    /* Parse out children */
    vector<json> children_json = res.deserialize<vector<json>>("children", vector<json>());
    for (auto& child : children_json) 
        add_child(new scene_node(resource(child)));
}

scene_node::~scene_node() {

    /* Remove node from parent's list of nodes */
    if (m_parent)
        m_parent->m_children.erase(m_name);

    /* Call exit callbacks on all scene nodes */
    if (m_in_scene) {
        for (auto& [id, component] : m_components.get_all())
            component->scene_exit();
    }

    /* Delete all children */
    for (auto it = m_children.cbegin(), next_it = it; it != m_children.cend(); it = next_it) {
        next_it++;
        delete it->second;
    }
}

void scene_node::update_node(float delta) {

    /* Moving the update logic here, it will make things more constistent */

    if (!m_enabled)
        return; /* Do not update self and children*/
    
    /* Update components */
    for (auto& [id, component] : m_components.get_all())
        component->update(delta);

    /* Process children */
    for (auto [name, child] : m_children) 
        child->update_node(delta);
}

void scene_node::prepare_draw(const glm::mat4x4& parent_transform) {

    if (!m_visible)
        return; /* Skip drawing if invisible */
    
    /* Prepare components */
    for (auto& [id, component] : m_components.get_all()) 
        component->prepare_draw(parent_transform * model_mat());

    /* Process children */
    for (auto [name, child] : m_children)
        child->prepare_draw(parent_transform * model_mat());
}

void scene_node::add_child(scene_node* node) {

    /* Check for invalid nodes */
    if (node == nullptr || node->m_type == node_type::ROOT) {
        std::cerr << "[ERROR] NULL node or ROOT node provided, will not be inserted as child!" << std::endl;
        return;
    }

    if (m_children.find(node->m_name) != m_children.end()) {
        std::cerr << "[WARNING] Duplicate node " << node->m_name << "! Will not be inserted as child!" << std::endl;
    }

    m_children.emplace(node->m_name, node);
    node->m_parent = this;

    /* Check for malformed scenes (Not a tree) */
    if (m_check_cycles())
        throw runtime_error("Dependency cycle in scene detected!");

    /* Check if desired node is in an active scene and if yes, propagate this to all its children */
    if (m_in_scene)
        node->m_on_scene_enter();
}

scene_node* scene_node::child(const string& name) const {

    vector<string> path_components = utils::split(name, "/");
    if (path_components.empty())
        throw std::logic_error("Invalid path provided, child " + name + " not found!");

    /* First one needs to be explicitly checked*/
    auto child_iter = m_children.find(path_components[0]);
    if (child_iter == m_children.cend())
        throw std::logic_error("Invalid path provided, child " + name + " not found!");
        
    auto child = child_iter->second;
    for (size_t i = 1; i < path_components.size(); i++) {
                 
        auto iter = child->m_children.find(path_components[i]); 
        if(iter == child->m_children.end())
            throw std::logic_error("Invalid path provided, child " + name + " not found!");

        child = iter->second;
    }

    return child;
}

mat4x4 scene_node::model_mat() const {

    return glm::scale(
        translate(identity<mat4x4>(), position) * toMat4(rotation),
        scale
    );
}

bool scene_node::visible(bool v) {

    if (v == m_visible)
        return m_visible;

    m_visible = v;

    /* Tell all the components that visibility changed */
    for (auto& [id, component] : m_components.get_all()) 
        component->visibility_changed();

    /* Process children */
    for (auto [name, child] : m_children)
        child->visible(v);

    return m_visible;
}

void scene_node::m_on_scene_enter() {

    /* Set in_scene flag */
    m_in_scene = true;

    /* All components now enter scene */
    for (auto& [id, component] : m_components.get_all())
        component->scene_enter();

    /* Every child has now also entered scene */
    for (auto& [name, child] : m_children)
        child->m_on_scene_enter();
}

bool scene_node::m_check_cycles() {

    /* Traverse towards root and check if parent can be reached again */
    scene_node* walk = m_parent;
    while (walk != nullptr) {

        if (walk->m_type == node_type::ROOT)
            return false;
    
        if (walk == this)
            return true;

        walk = walk->m_parent;
    }

    return false;
}

