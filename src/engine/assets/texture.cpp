#include "texture.hpp"
#include <GL/gl.h>
#include <algorithm>
#include <stdexcept>
#include "../utils/project_settings.hpp"
#include "../rendering/renderer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../lib/stb/stb_image.h"

using namespace std;
using namespace utils;
using namespace assets;

texture::texture()
    : m_texture_obj(0), m_texture_index(-1), m_w(0), m_h(0), m_channels(0) {}

texture::texture(const std::string name) 
    : m_texture_index(-1) {
    
    uint8_t* img_data = stbi_load(name.c_str(), &m_w, &m_h, &m_channels, STBI_rgb_alpha);
    if (img_data == NULL)
        throw std::runtime_error("Image " + name + " not found or corrupted");

    /* Create OpenGL texture object */ 
    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture_obj);

    glTextureParameteri(m_texture_obj, GL_TEXTURE_MIN_FILTER, project_settings::tex_min_filter());
    glTextureParameteri(m_texture_obj, GL_TEXTURE_MAG_FILTER, project_settings::tex_mag_filter());
    glTextureParameteri(m_texture_obj, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_texture_obj, GL_TEXTURE_WRAP_T, GL_REPEAT);
            
    /* Calculate number of mipmap levels */
    int mip_levels = static_cast<int>(min(5.0f, log2f(static_cast<float>(max(m_w, m_h)))));

    /* Send image to OpenGL */
    glTextureStorage2D(m_texture_obj, mip_levels, GL_RGBA8, m_w, m_h);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glTextureSubImage2D(
        m_texture_obj, 
        0, 0, 0, 
        m_w, m_h, 
        GL_RGBA, 
        GL_UNSIGNED_BYTE, img_data
    );

    /* Create b&w image, not just R */
    if (m_channels == 1) {
        GLint swizzle[] = { GL_RED, GL_RED, GL_RED, GL_RED, GL_RED };
        glTextureParameteriv(m_texture_obj, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    }

    glGenerateTextureMipmap(m_texture_obj);
    m_texture_handle = glGetTextureHandleARB(m_texture_obj);
    stbi_image_free(img_data);  
}
 
void texture::use() {
    
    /* Texture is already in use, no need to redo */
    if (m_texture_index >= 0)
        return;

    /* Make texture resindent & pass it to the renderer */
    glMakeTextureHandleResidentARB(m_texture_handle);

    auto [handle, offset] = rendering::renderer::instance()->texture_allocator().alloc_buffer(sizeof(m_texture_handle));
    rendering::renderer::instance()->texture_allocator().buffer_data(handle, sizeof(m_texture_handle), &m_texture_handle);

    /* Calculate index */
    m_texture_index = offset / sizeof(m_texture_handle);
    m_buffer_handle = handle;
}

texture::~texture() {
     
    /* If rexture was in use, unbind it */
    if (m_texture_index >= 0) {
     
        glMakeTextureHandleNonResidentARB(m_texture_handle);
        rendering::renderer::instance()->texture_allocator().free_buffer(m_buffer_handle);
    }

    glDeleteTextures(1, &m_texture_obj);
}
