#pragma once
#include "../nodes/scene_node.hpp"
#include "asset.hpp"
#include <string>


namespace assets {
    class scene : asset {
        public:
            scene(std::string path);
            nodes::scene_node* instantiate();
    };
};