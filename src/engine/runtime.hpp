#pragma once

#include "nodes/camera.hpp"
#include "nodes/scene_node.hpp"
#include "physics/physics.hpp"
#include "rendering/renderer.hpp"
#include "game_window.hpp"
#include "events.hpp"

class engine_runtime {
   
    public:
        engine_runtime(game_window* window);
        void start(); 

        static engine_runtime* instance();
        
        inline nodes::scene_node* root_node() const { return _root_node; }
        inline game_window* window() const { return _window; }
        
        nodes::scene_node* root_node(nodes::scene_node* node);

    private:
        static engine_runtime* _instance;
        
        nodes::scene_node* _root_node;
        game_window* _window;

        nodes::camera* _main_camera;
        physics::physics_engine* _physics;
        rendering::renderer* _renderer;
        events* _events; /* So the instance doesn't live on the stack*/

        void _teardown();
};
