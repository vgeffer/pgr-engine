#include "runtime.hpp"
#include "events.hpp"
#include "nodes/scene_node.hpp"
#include "physics/physics.hpp"
#include "rendering/renderer.hpp"
#include "utils/global_settings.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#include <deque>
#include <iterator>

using namespace std;
using namespace nodes;
using namespace utils;
using namespace physics;
using namespace rendering;
using namespace std::chrono;

engine_runtime::engine_runtime(game_window* window) 
    : _window(window) {

    _events = new events();
    _events->apply_callbacks(window);

    _renderer = new renderer();
    _physics = new physics_engine();
    _instance = this;
};
    
engine_runtime* engine_runtime::instance() {
    return _instance;
}

void engine_runtime::start() {           
        
    /* Setup timekeeping */
    steady_clock::time_point tp_prev = steady_clock::now(), 
                             tp_now;

    float physics_delta = 0;
    float physics_interval = global_settings::physics_interval();

    /* TODO: Check if main camera is bound. If not... create one */

    /* Mainloop */
    while (!_window->props().is_closing) {
        
        _events->process_frame();

        /* Calculate time elapsed since last frame */
        tp_now = steady_clock::now();
        float elapsed = (tp_now - tp_prev).count() / 1000.0f;
        tp_prev = tp_now;
        
        physics_delta += elapsed;

        /* Physics */
        while (physics_delta >= physics_interval) {
            _physics->tick(physics_interval);
            physics_delta -= physics_interval;
        }

        /* Logic */
        deque<scene_node*> bfs_queue = deque<scene_node*>(1, _root_node);
        while (!bfs_queue.empty()) {
            
            scene_node* current = bfs_queue.front();
            if (!current->enabled()) {
                bfs_queue.pop_front();
                continue; /* Do not update self and children*/
            }

            /* Lerp on positions */
            current->node_update(elapsed);

            /* Append children to queue*/
            if (current->children().size() > 0)
                move(
                    current->children().begin(),
                    current->children().end(),
                    back_inserter(bfs_queue)
                );

            bfs_queue.pop_front();
        }

        /* Clear buffers */
        glfwSwapBuffers(_window->props().glfw_handle);

        /* Render & postprocess */
        _renderer->draw_scene(*_main_camera);
    }
    
    /* Delete OpenGL objects */
    _teardown();
}

scene_node* engine_runtime::root_node(scene_node* root) {

    /* Let renderer process the scene */
    _renderer->load_scene(root);

    /* Recompute Bounding Volume Hierarchy for new scene */
    _physics->recompute_bv_hierarchy(root);

    return _root_node = root;
}
    
void engine_runtime::_teardown() {

    /* Delete current scene */

    delete _events;
    delete _renderer;
}
