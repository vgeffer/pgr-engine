#pragma once

#include "../rendering/mesh.hpp"
#include <memory>
#include "asset.hpp"
#include "../../lib/json/json.hpp"


namespace assets {

    class terrain_mesh : public asset, public rendering::mesh {

        public:
            terrain_mesh(std::string path);
            terrain_mesh(nlohmann::basic_json<>& src);

            inline uint16_t res_horizontal() const { return _res_horizontal; }
            inline uint16_t res_vertical() const {return _res_vertical; } 

        private: 
            uint16_t _res_horizontal, _res_vertical;
            std::shared_ptr<assets::shader_stage> _terain_vertex_stage;
    };
};