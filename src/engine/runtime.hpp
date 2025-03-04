#pragma once

#include "nodes/scene_node.hpp"
#include "rendering/renderer.hpp"
#include "game_window.hpp"

const float physics_interval = 10.0f; /* in ms */

class engine_runtime {
   
    public:
        engine_runtime(game_window* window);
        void start(); 

        static engine_runtime* instance();
        
        nodes::scene_node* root_node() const;
        game_window* window() const;
        
        nodes::scene_node* root_node(nodes::scene_node* node);
        

    private:
        static engine_runtime* _instance;
        
        nodes::scene_node* _root_node;
        game_window* _window;
        rendering::renderer* _renderer;

        void _teardown();
};
