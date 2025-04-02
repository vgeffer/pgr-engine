#include "runtime.hpp"
#include "assets/asset.hpp"
#include "events.hpp"
#include "game_window.hpp"
#include "nodes/scene_node.hpp"
#include "physics/physics.hpp"
#include "rendering/mesh.hpp"
#include "rendering/renderer.hpp"
#include "utils/observer_ptr.hpp"
#include "utils/project_settings.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

using namespace glm;
using namespace std;
using namespace nodes;
using namespace utils;
using namespace physics;
using namespace rendering;
using namespace std::chrono;

engine_runtime::engine_runtime(game_window& window) 
    : _root_node(new scene_node(scene_node::node_type::ROOT)), _window(window), _main_camera(nullptr) {

    _events = make_unique<events>();
    _renderer = make_unique<renderer>();
    _physics = make_unique<physics_engine>();

    _events->apply_callbacks(window);
    _instance = this;

    glfwSetWindowCloseCallback(window.props().glfw_handle, [](GLFWwindow*) { engine_runtime::_instance->_window.close(); });
    glViewport(0, 0, window.props().current_mode.w, window.props().current_mode.h);
};

engine_runtime::~engine_runtime() {

    /* Invalidate all cached objects so they will delete themselves */
    assets::asset::invalidate();

    _recursive_scene_delete(_root_node);
}
    
engine_runtime* engine_runtime::instance() {
    return _instance;
}

void engine_runtime::start() {           
        
    /* Setup timekeeping */
    system_clock::time_point tp_prev = system_clock::now(), 
                             tp_now;

    float physics_delta = 0.0f;
    float physics_interval = project_settings::physics_interval();

    /* Mainloop */
    while (!_window.props().is_closing) {
        
        _events->process_frame();

        /* Calculate time elapsed since last frame */
        tp_now = system_clock::now();
        float elapsed = duration_cast<milliseconds>(tp_now - tp_prev).count() / 1000.0f;
        tp_prev = tp_now;        

        physics_delta += elapsed;

        /* Physics */
        while (physics_delta >= physics_interval) {
            _physics->tick(physics_interval);
            physics_delta -= physics_interval;
        }

        /* Logic */
        const mat4x4 ident = identity<mat4x4>();
        _recursive_scene_update(elapsed, _root_node, ident);

        /* Render & postprocess */
        if (_main_camera)
            _renderer->draw_scene(*_main_camera);

        /* Display new frame */
        glfwSwapBuffers(_window.props().glfw_handle);

        /* Debug */
        if (_events->is_key_pressed(key_code::ESC))
            _window.close();
    }
}

scene_node* engine_runtime::root_node(scene_node* root) {

    /* Delete children of root node */
    for (auto& child : _root_node->children())
        _recursive_scene_delete(child);

    /* Add node to scene - recursively adds children to scene */
    _root_node->add_child(root);       

    /* Recompute Bounding Volume Hierarchy for new scene */
    _physics->recompute_bv_hierarchy(root);

    /* Return */
    return _root_node;
}

void engine_runtime::_recursive_scene_update(const float elapsed, scene_node* node, const mat4x4& parent_transform) {

    if (_root_node == nullptr)
        return;

    if (!node->enabled())
        return; /* Do not update self and children*/
    
    /* Lerp on positions */
    node->node_update(elapsed);

    /* Frustrum culling on the object - if renderable */
    if (node->has_component<mesh_instance>()) {
        
        auto mesh = node->component<mesh_instance>();
        mesh->request_draw(node->model_mat());
    }
        
    /* Append children to queue*/
    for (auto child : node->children()) 
        _recursive_scene_update(elapsed, child, node->model_mat() * parent_transform);
}

void engine_runtime::_recursive_scene_delete(scene_node* node) {

    if (node == nullptr)
        return;

    /* Do a simple postorder deleting nodes */
    for (auto child : node->children())
        _recursive_scene_delete(child);

    delete node;
}
