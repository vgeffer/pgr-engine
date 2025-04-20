#include "runtime.hpp"
#include "assets/loader.hpp"
#include "assets/scene.hpp"
#include "assets/shader.hpp"
#include "events.hpp"
#include "game_window.hpp"
#include "scene/scene_node.hpp"
#include "physics/physics.hpp"
#include "rendering/renderer.hpp"
#include "utils/logger.hpp"
#include "utils/project_settings.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

using namespace glm;
using namespace std;
using namespace scene;
using namespace utils;
using namespace physics;
using namespace rendering;
using namespace std::chrono;

/* Tie singletons with the app */
events g_events = events();


engine_runtime::engine_runtime(game_window& window) 
    : _root_node(new scene_node("GLOBAL_ROOT", scene_node::node_type::ROOT)), _window(window) {

    _instance = this;



    _renderer = make_unique<renderer>();
    _physics = make_unique<physics_engine>();

    g_events.apply_callbacks(window);

    glfwSetWindowCloseCallback(window.props().glfw_handle, [](GLFWwindow*) { engine_runtime::_instance->_window.close(); });
    glViewport(0, 0, window.props().current_mode.size().x, window.props().current_mode.size().y);
};

engine_runtime::~engine_runtime() {

    /* Delete the scene */
    delete _root_node;
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

    /* Load the initial scene */
    auto initial_scene = assets::loader::load<assets::scene_template>(project_settings::default_scene_path());
    root_node(initial_scene->instantiate());

    /* Check if renderer has a valid camera */
    if (!_renderer->has_active_camera())
        logger::error << "No main camera found in the scene! For rendering to work, you'll need to set one up manually" << std::endl;

    /* Mainloop */
    while (!_window.props().is_closing) {
        
        g_events.process_frame();

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
        
        if (_root_node != nullptr) {
            _root_node->update_node(elapsed);
            _root_node->prepare_draw(ident);
        }

        /* Render & postprocess */
        _renderer->draw_scene();

        /* Display new frame */
        glfwSwapBuffers(_window.props().glfw_handle);
    }
}

scene_node* engine_runtime::root_node(scene_node* root) {

    /* Delete children of root node */
    for (auto& [name, child] : _root_node->children())
        delete child;

    /* Add node to scene - recursively adds children to scene */
    _root_node->add_child(root);       

    /* Recompute Bounding Volume Hierarchy for new scene */
    _physics->recompute_bv_hierarchy(root);

    /* Return */
    return _root_node;
}
