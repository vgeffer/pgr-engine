#include "texture.hpp"
#include <stdexcept>
#include "../utils/cahce.hpp"

using namespace assets;

texture::texture(std::string name) {
    
    if (core::cache::has_item(name)) {
        /* TODO: Init from cache */
        return;
    }

    uint8_t* data = stbi_load(name.c_str(), &w, &h, &channels, 4);
    if (data == NULL)
        throw std::runtime_error("Image " + name + " not found or corrupted");

    /* Create OpenGL texture object */ 
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
            
    /* Set texture parameters -> TODO: allow user selection */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            
    /* Send image to OpenGL */
    glTexImage2D(
        GL_TEXTURE_2D, 0, 
        GL_RGBA, 
        w, h, 0, 
        (channels == 4 ? GL_RGBA : GL_RGB), 
        GL_UNSIGNED_BYTE, data
    );
            
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);   
    stbi_image_free(data);
}
 
texture::~texture() {
        
    glDeleteTextures(1, &tex_id);
}
