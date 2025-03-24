#include "texture.hpp"
#include <stdexcept>
#include "../utils/project_settings.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../lib/stb/stb_image.h"

using namespace std;
using namespace utils;
using namespace assets;


texture::texture(const std::string name) {
    
    uint8_t* data = stbi_load(name.c_str(), &_w, &_h, &_channels, 4);
    if (data == NULL)
        throw std::runtime_error("Image " + name + " not found or corrupted");

    /* Create OpenGL texture object */ 
    glGenTextures(1, &_tex_id);
    glBindTexture(GL_TEXTURE_2D, _tex_id);
            
    /* Set texture parameters -> TODO: allow user selection */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, project_settings::tex_min_filter());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, project_settings::tex_mag_filter());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            
    /* Send image to OpenGL */
    glTexImage2D(
        GL_TEXTURE_2D, 0, 
        GL_RGBA, 
        _w, _h, 0, 
        (_channels == 4 ? GL_RGBA : GL_RGB), 
        GL_UNSIGNED_BYTE, data
    );
            
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);   
    stbi_image_free(data);  
}
 
texture::~texture() {
        
    glDeleteTextures(1, &_tex_id);
}
