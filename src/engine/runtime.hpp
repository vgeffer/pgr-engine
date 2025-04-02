#pragma once

#include "nodes/camera.hpp"
#include "nodes/scene_node.hpp"
#include "physics/physics.hpp"
#include "rendering/renderer.hpp"
#include "game_window.hpp"
#include "events.hpp"

#include <glm/fwd.hpp>
#include <memory>

class engine_runtime {
   
    public:
        engine_runtime(game_window& window);
        ~engine_runtime();
        void start(); 

        static engine_runtime* instance();
        
        inline nodes::scene_node* root_node() const { return _root_node; }
        inline game_window& window() const { return _window; }
        inline void set_main_camera(nodes::camera* camera) { _main_camera = camera; }

        nodes::scene_node* root_node(nodes::scene_node* node);

    private:
        inline static engine_runtime* _instance = nullptr;
        nodes::scene_node* _root_node;
        game_window& _window;

        nodes::camera* _main_camera;
        std::unique_ptr<physics::physics_engine> _physics;
        std::unique_ptr<rendering::renderer> _renderer;
        std::unique_ptr<events> _events;

        void _teardown();
        void _recursive_scene_update(const float elapsed, nodes::scene_node* node, const glm::mat4x4& parent_transform);
        void _recursive_scene_delete(nodes::scene_node* node);
        
};
