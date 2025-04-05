#pragma once
#include "asset.hpp"
#include "../utils/observer_ptr.hpp"
#include "../rendering/material.hpp"
#include <string>



namespace assets {

    /* "Material Template" */
    class material_template : public asset {
        
        public:
            material_template(std::string path);
            ~material_template();

            utils::observer_ptr<rendering::material>& instantiate();
        
        private:
            utils::observable_ptr<rendering::material> _mat;
    };
}