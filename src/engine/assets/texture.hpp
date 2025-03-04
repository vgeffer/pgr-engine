#pragma once
#include "../../lib/glad/glad.h"
#include "../../lib/stb/stb_image.h"
#include "asset.hpp"
#include <string>

namespace assets {
    class texture : public asset {

        public: 
            texture(std::string name) ; 
            ~texture();    

        private:
            GLuint tex_id;
            int w, h, channels;
    };
}