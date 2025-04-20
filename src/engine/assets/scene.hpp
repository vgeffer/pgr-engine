#pragma once
#include "../scene/scene_node.hpp"
#include "asset.hpp"
#include <string>


namespace assets {
    class scene_template : public asset {
        public:
            scene_template(const std::string path);
            ~scene_template() override = default;

            scene::scene_node* instantiate();
        private:
            utils::resource m_scene_res;
    };
};