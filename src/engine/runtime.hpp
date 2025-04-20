#pragma once

#include "scene/scene_node.hpp"
#include "physics/physics.hpp"
#include "rendering/renderer.hpp"
#include "game_window.hpp"

#include <glm/fwd.hpp>
#include <memory>

class engine_runtime {
   
    public:
        engine_runtime(game_window& window);
        ~engine_runtime();
        void start(); 

        static engine_runtime* instance();
        
        inline scene::scene_node* root_node() const { return _root_node; }
        scene::scene_node* root_node(scene::scene_node* node);

        inline game_window& window() const { return _window; }

    private:
        inline static engine_runtime* _instance = nullptr;
        scene::scene_node* _root_node;
        game_window& _window;

        std::unique_ptr<physics::physics_engine> _physics;
        std::unique_ptr<rendering::renderer> _renderer;
};
