#include "scene_node.hpp"
#include <glm/detail/type_quat.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

using namespace std;
using namespace glm;
using namespace scene;
using namespace utils;
using namespace nlohmann;

scene_node::scene_node()
    : scene_node(scene_node::node_type::GENERIC) {}

scene_node::scene_node(scene_node::node_type type)
    : _parent(nullptr), _enabled(true), _in_scene(type == node_type::ROOT), _type(type), _position(vec3(0,0,0)), _scale(vec3(1,1,1)), 
      _rotation(quat(0,0,0,0)) {}

scene_node::scene_node(const resource& res)
    : _parent(nullptr), _enabled(true), _in_scene(false), _type(node_type::GENERIC) {

    _position = res.deserialize<vec3>("position", vec3(0, 0, 0));
    _rotation = res.deserialize<quat>("rotation", quat(0, 0, 0, 0));
    _scale = res.deserialize<vec3>("scale", vec3(1, 1, 1));

    _children = res.deserialize<vector<scene_node*>>("children", vector<scene_node*>());

    /* Parse out node components */
    json component_map = res.deserialize<json>("components", {});
    for (auto [name, component] : component_map.items()) {

        auto spawner = _internal::component_registry::registered_components.find(name);
        if (spawner == _internal::component_registry::registered_components.end()) {
            std::cerr << "Unknown component " << name << " skipping!\n";     
            continue;
        }

        /* Call the spawner fn */
        spawner->second(*this, component);
    }
}

scene_node::~scene_node() {

    if (!_in_scene)
        return;
    
    for (auto& [id, component] : _components.get_all())
        component->scene_exit();
}

void scene_node::node_update(float delta) {

    for (auto& [id, component]: _components.get_all())
        component->update(delta);
    update(delta);
}

void scene_node::add_child(scene_node* node) {

    _children.push_back(node);
    node->_parent = this;

    /* Check for malformed scenes (Not a tree) */
    if (_check_cycles())
        throw runtime_error("Dependency cycle in scene detected!");

    /* Check if desired node is in an active scene and if yes, propagate this to all its children */
    if (_in_scene)
        node->_on_scene_enter();
}

scene_node* scene_node::child(size_t index) const {

    if (_children.size() >= index)
        throw runtime_error("Child index out of bounds");

    return _children.at(index);
}

mat4x4 scene_node::model_mat() const {

    return glm::scale(
        translate(identity<mat4x4>(), _position) * toMat4(_rotation),
        _scale
    );
}

void scene_node::_on_scene_enter() {

    /* Set in_scene flag */
    _in_scene = true;

    /* All components now enter scene */
    for (auto& [id, component] : _components.get_all())
        component->scene_enter();

    /* Every child has now also entered scene */
    for (auto& child : _children)
        child->_on_scene_enter();
}

bool scene_node::_check_cycles() {

    /* Traverse towards root and check if parent can be reached again */

    scene_node* walk = _parent;
    while (walk != nullptr) {

        if (walk->_type == node_type::ROOT)
            return false;
    
        if (walk == this)
            return true;

        walk = walk->_parent;
    }

    return false;
}

