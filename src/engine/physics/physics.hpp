#pragma once
#include "../scene/scene_node.hpp"
#include <glm/fwd.hpp>


namespace physics {


    class physics_body {

        protected:
            glm::vec3 velocity;
    };

    class physics_engine {

        public:
            void tick(float interval) {}
            void recompute_bv_hierarchy(scene::scene_node* root_node) {}

        private:
            inline static physics_engine* _instance = nullptr;
    };
};