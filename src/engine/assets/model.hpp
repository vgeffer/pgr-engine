#pragma once 
#include "../rendering/mesh.hpp"
#include <string>

namespace assets {
    class model : public asset, public rendering::mesh {
       
        public:
            explicit model(std::string path);
            ~model() = default;
    };
}
