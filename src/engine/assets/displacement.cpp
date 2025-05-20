#include "displacement.hpp"
#include "../rendering/renderer.hpp"
#include <array>
#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include "../../lib/stb/stb_image.h"

#define DISPLACEMENT_HIEGHT_MODIFIER 6.0f

using namespace std;
using namespace utils;
using namespace assets;
using namespace rendering;

displacement::displacement(const std::string& path)
    : mesh() {

    /* Load the heightmap */
    m_heightmap = stbi_load(path.c_str(), &m_w, &m_h, nullptr, STBI_grey);
    if (m_heightmap == nullptr)
        throw std::runtime_error("Image " + path + " not found or corrupted");

    /* Generate vertices */
    vector<mesh::vertex> vertices;
    vertices.reserve(m_w * m_h);

    for (int z = 0; z < m_h; z++) {
        for (int x = 0; x < m_w; x++) {

            vertices.emplace_back(
                glm::vec3(static_cast<float>(x), static_cast<float>(m_heightmap[z * m_w  + x]) / DISPLACEMENT_HIEGHT_MODIFIER, 
                static_cast<float>(z)), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2( 
                    static_cast<float>(x) / static_cast<float>(m_w) * 12.0f,
                    static_cast<float>(z) / static_cast<float>(m_h) * 12.0f 
                )
            ); 
        }
    }

    /* Generate indices */
    vector<int> indices;
    indices.reserve((m_w - 1) * (m_h - 1) * 6);

    for (int z = 0; z < m_h - 1; z++) {
        for (int x = 0; x < m_w - 1; x++) {
        
            uint bot_left = z * m_w + x, 
                 bot_right = z * m_w + (x + 1);
            uint top_left = (z + 1) * m_w + x,
                 top_right = (z + 1) * m_w + (x + 1);
            
            /* Bottom-left triangle */
            indices.emplace_back(top_left);
            indices.emplace_back(bot_right); 
            indices.emplace_back(bot_left); 
            
            /* Top-right triangle */
            indices.emplace_back(top_left);
            indices.emplace_back(top_right); 
            indices.emplace_back(bot_right);
        }
    }

    /* Calculate normals */
    for (size_t i = 0; i < indices.size(); i += 3) {

        glm::vec3 v1 = vertices[indices[i + 1]].position - vertices[indices[i]].position;
        glm::vec3 v2 = vertices[indices[i + 2]].position - vertices[indices[i]].position;
        glm::vec3 n = glm::normalize(glm::cross(v1, v2));

        vertices[indices[i]].normal += n;
        vertices[indices[i + 1]].normal += n;
        vertices[indices[i + 2]].normal += n;
    }

    /* Renormalize all normals */
    for (size_t i = 0; i < vertices.size(); i++)
        vertices[i].normal = glm::normalize(vertices[i].normal);

    /* Reserve buffer for vertices */
    auto [vert_handle, vert_offset] = renderer::instance()->vertex_allocator().alloc_buffer(vertices.size() * sizeof(vertices[0]));
    
    /* Upload vertices to the GPU */
    renderer::instance()->vertex_allocator().buffer_data(vert_handle, vertices.size() * sizeof(vertices[0]), vertices.data());
    m_vert_handle = vert_handle;
    m_first_vertex = vert_offset / sizeof(vertices[0]);

    /* Reserve buffer for indices */
    auto [elem_handle, elem_offset] = renderer::instance()->element_allocator().alloc_buffer(indices.size() * sizeof(indices[0]));
            
    /* Upload indices to the GPU */
    renderer::instance()->element_allocator().buffer_data(elem_handle, indices.size() * sizeof(indices[0]), indices.data());
    m_elem_handle = elem_handle;
    m_first_index = elem_offset / sizeof(indices[0]);   
    m_element_count = indices.size();
}

displacement::~displacement() {
     
   stbi_image_free(m_heightmap);
}

float displacement::height_at(const glm::vec2& pos) const {

    /* Get points to sample at pos - wrap around if out of terrain */
    glm::ivec2 sample_pt_x = glm::ivec2(glm::floor(pos.x), glm::ceil(pos.x));
    glm::ivec2 sample_pt_y = glm::ivec2(glm::floor(pos.y), glm::ceil(pos.y));

    if (pos.x < 0 || pos.x > static_cast<float>(m_w) || pos.y < 0 || pos.y > static_cast<float>(m_h))
        return INFINITY;

    std::array<float, 4> img_samples {
        static_cast<float>(m_heightmap[sample_pt_y.x * m_w + sample_pt_x.x]) / DISPLACEMENT_HIEGHT_MODIFIER,
        static_cast<float>(m_heightmap[sample_pt_y.x * m_w + sample_pt_x.y]) / DISPLACEMENT_HIEGHT_MODIFIER,
        static_cast<float>(m_heightmap[sample_pt_y.y * m_w + sample_pt_x.x]) / DISPLACEMENT_HIEGHT_MODIFIER,
        static_cast<float>(m_heightmap[sample_pt_y.y * m_w + sample_pt_x.y]) / DISPLACEMENT_HIEGHT_MODIFIER
    };

    /* Interpolate between points. If the x is the same, skip x-interpolation */
    std::array<float, 2> x_interpolated { img_samples[0], img_samples[2] };
    if (sample_pt_x.x != sample_pt_x.y) {

        x_interpolated[0] = (((static_cast<float>(sample_pt_x.y) - pos.x) / static_cast<float>(sample_pt_x.y - sample_pt_x.x)) * img_samples[0]) +
                            (((pos.x - static_cast<float>(sample_pt_x.x)) / static_cast<float>(sample_pt_x.y - sample_pt_x.x)) * img_samples[1]);

        x_interpolated[1] = (((static_cast<float>(sample_pt_x.y) - pos.x) / static_cast<float>(sample_pt_x.y - sample_pt_x.x)) * img_samples[2]) +
                            (((pos.x - static_cast<float>(sample_pt_x.x)) / static_cast<float>(sample_pt_x.y - sample_pt_x.x)) * img_samples[3]);
    }

    /* Do y-interpolation */
    if (sample_pt_y.x != sample_pt_y.y)
        return x_interpolated[0];

    return (((static_cast<float>(sample_pt_y.y) - pos.y) / static_cast<float>(sample_pt_y.y - sample_pt_y.x)) * x_interpolated[0]) +
           (((pos.y - static_cast<float>(sample_pt_y.x)) / static_cast<float>(sample_pt_y.y - sample_pt_y.x)) * x_interpolated[1]);
}

float displacement::height_at(const glm::vec3& pos) const {

    return height_at(glm::vec2(pos.x, pos.z));
}
