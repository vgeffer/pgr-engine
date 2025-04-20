///
/// @file displacement.hpp
/// @author geffevil
/// @brief Resource providing dynamically generated displacement meshes
///
#pragma once

#include "../rendering/mesh.hpp"
#include <memory>
#include "asset.hpp"

namespace assets {

    class displacement : public asset, public rendering::mesh {

        public:
            displacement(std::string path);
            displacement(const utils::resource& src) {}
            displacement(displacement&&) = default;

            inline uint16_t res_horizontal() const { return _res_horizontal; }
            inline uint16_t res_vertical() const {return _res_vertical; } 

        private: 
            uint16_t _res_horizontal, _res_vertical;
            std::shared_ptr<assets::shader_stage> _terain_vertex_stage;
    };
};

MAKE_JSON_PARSABLE(assets::displacement, [] (const utils::resource& res){
    return assets::displacement(res);
});