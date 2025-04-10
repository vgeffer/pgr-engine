#pragma once 
#include "../rendering/mesh.hpp"
#include <string>

namespace assets {
    class model : public asset, public rendering::mesh {
       
        public:
            model(std::string path, bool parse_material = true);
            ~model() override = default;
    };
}
