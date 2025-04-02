#include "scene_node.hpp"
#include <glm/fwd.hpp>
#include <stdexcept>

using namespace std;
using namespace glm;
using namespace nodes;


static mat4x4 _calc_model_mat(const vec3& pos, const quat& rot, const vec3& scale) {

    return mat4x4();
}
        
scene_node::scene_node()
    : scene_node(scene_node::node_type::GENERIC) {}

scene_node::scene_node(scene_node::node_type type)
    : _parent(nullptr), _enabled(true), _in_scene(type == node_type::ROOT), _type(type), _position(vec3(0,0,0)), _scale(vec3(1,1,1)), 
      _rotation(quat(0,0,0,0)), _model_mat(_calc_model_mat(vec3(0,0,0), quat(0,0,0,0), vec3(1,1,1))) {}

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

vec3 scene_node::position(const glm::vec3& position) {
    _model_mat = _calc_model_mat(position, _rotation, _scale);
    return _position = position;
}

quat scene_node::rotation(const glm::quat& rotation) {
    _model_mat = _calc_model_mat(_position, rotation, _scale);
    return _rotation = rotation;
}

vec3 scene_node::scale(const glm::vec3& scale) {

    _model_mat = _calc_model_mat(_position, _rotation, scale);
    /* TODO: If non-uniform, recompute normals */
    return _scale = scale;
}

void scene_node::_on_scene_enter() {

    /* Set in_scene flag */
    _in_scene = true;

    /* All components now enter scene */
    for (auto& [id, component] : _components.get_all())
        component->on_scene_enter();

    /* Every child has now also entered scene */
    for (auto& child : _children)
        child->_on_scene_enter();
}

bool scene_node::_check_cycles() {

    /* Traverse towards root and check if parent can be reached again */

    scene_node* walk = _parent;
    while (walk != nullptr) {

        /* TODO: Designate root node */
        if (walk->_type == node_type::ROOT)
            return false;
    
        if (walk == this)
            return true;
    }

    return false;
}

