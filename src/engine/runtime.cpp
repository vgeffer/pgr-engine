#include "runtime.hpp"
#include "nodes/scene_node.hpp"
#include "rendering/renderer.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#include <deque>
#include <iterator>

using namespace std;
using namespace nodes;
using namespace rendering;
using namespace std::chrono;

engine_runtime::engine_runtime(game_window* window) 
    : _window(window) {

    _renderer = new renderer();
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

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
        
    /* Setup backface culling */
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    /* Mainloop */
    while (!_window->props().is_closing) {
        
        glfwPollEvents();

        /* Calculate time elapsed since last frame */
        tp_now = steady_clock::now();
        float elapsed = (tp_now - tp_prev).count() / 1000.0f;
        tp_prev = tp_now;
        
        physics_delta += elapsed;

        /* Physics */
        while (physics_delta >= physics_interval) {
            /* TODO: physics_update */
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
            current->update(elapsed);

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Render */
        _renderer->draw_scene(_root_node);

        /* Postprocess - if enabled */
    }
    
    /* Delete OpenGL objects */
    _teardown();
}

scene_node* engine_runtime::root_node() const {
    return _root_node;
}

game_window* engine_runtime::window() const {
    return _window;
}

scene_node* engine_runtime::root_node(scene_node* node) {

    /* TODO: update renderer's info about static objects */
    return _root_node = node;
}
    
void engine_runtime::_teardown() {
          

    delete _renderer;
}
