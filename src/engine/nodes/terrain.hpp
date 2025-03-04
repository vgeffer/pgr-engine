#pragma once

#include "../rendering/mesh.hpp"
#include "scene_node.hpp"
#include <vector>



namespace nodes {
    class terrain : public scene_node, public rendering::mesh {

        public:
            terrain(uint32_t res_h, uint32_t res_v);
            terrain(uint32_t res_h, uint32_t res_v, std::vector<float> pt_displacements);

        private: 
            uint32_t _res_horizontal, _res_vertical;
    };
};