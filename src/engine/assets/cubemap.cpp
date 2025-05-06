#include "cubemap.hpp"
#include <GL/gl.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include "../utils/project_settings.hpp"
#include "../utils/resource.hpp"
#include "../../lib/stb/stb_image.h"

using namespace std;
using namespace utils;
using namespace assets;


cubemap::cubemap()
    : m_cubemap_obj(0), m_w(0), m_h(0), m_channels(0) {}

cubemap::cubemap(const std::string& name) {
    
    /* Load resource of texture infos */
    resource cubemap_descriptor = resource(name);
    vector<string> face_filenames = cubemap_descriptor.deserialize<vector<string>>("cubemap/faces");

    if (face_filenames.size() < 6)
        throw std::logic_error("Cubemap " + name + " contains less than 6 defined face textures");

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_cubemap_obj);    

    /* Explicitly load the first texture */
    uint8_t* first_img_data = stbi_load(face_filenames[0].c_str(), &m_w, &m_h, &m_channels, STBI_rgb_alpha);
    if (first_img_data == nullptr)
        throw std::runtime_error("Image " + face_filenames[0] + " not found or corrupted");

    /* Calculate number of mipmap levels */
    int mip_levels = static_cast<int>(min(5.0f, log2f(static_cast<float>(max(m_w, m_h)))));

    glTextureStorage2D(m_cubemap_obj, mip_levels, GL_RGBA8, m_w, m_h);
    glTextureSubImage3D(
        m_cubemap_obj, 
        0, 0, 0, 0, 
        m_w, m_h, 1, 
        GL_RGBA, GL_UNSIGNED_BYTE, first_img_data
    );

    glTextureParameteri(m_cubemap_obj, GL_TEXTURE_MIN_FILTER, project_settings::tex_min_filter());
    glTextureParameteri(m_cubemap_obj, GL_TEXTURE_MAG_FILTER, project_settings::tex_mag_filter());
    glTextureParameteri(m_cubemap_obj, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_cubemap_obj, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_cubemap_obj, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    /* Load and process sides */
    for (int i = 1; i < 6; i++) {
        
        int face_w, face_h, face_channels;
        uint8_t* img_data = stbi_load(face_filenames[i].c_str(), &face_w, &face_h, &face_channels, STBI_rgb_alpha);
        if (img_data == nullptr)
            throw std::runtime_error("Image " + face_filenames[i] + " not found or corrupted");

        if (face_w != m_w || face_h != m_h) 
            throw std::logic_error("Image " + face_filenames[i] + " has different size than others");

        glTextureSubImage3D(
            m_cubemap_obj, 
            0, 0, 0, i, 
            face_w, face_h, 1, 
            GL_RGBA, GL_UNSIGNED_BYTE, img_data
        );
        stbi_image_free(img_data);
    }

    glGenerateTextureMipmap(m_cubemap_obj);
}

cubemap::~cubemap() {

    glDeleteTextures(1, &m_cubemap_obj);
}
