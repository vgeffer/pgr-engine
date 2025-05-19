///
/// @file billboard.hpp
/// @author geffevil
///
#pragma once
#include "../mesh.hpp"
#include "../renderer.hpp"
#include <array>

using namespace rendering;

/// @brief Built in billboard mesh
/// @see rendering::mesh
class billboard : public mesh {

    public:
        billboard()
            : mesh() {

            /* Reserve buffer for the vertices */
            auto [vert_handle, vert_offset] = renderer::instance()->vertex_allocator().alloc_buffer(
                s_billboard_verices.size() * sizeof(s_billboard_verices[0])
            );

            /* Upload the data */
            renderer::instance()->vertex_allocator().buffer_data(
                vert_handle, s_billboard_verices.size() * sizeof(s_billboard_verices[0]), 
                s_billboard_verices.data()
            );
            m_vert_handle = vert_handle;
            m_first_vertex = vert_offset / sizeof(s_billboard_verices[0]);
        
            /* Reserve buffer for the indices */
            auto [eles_handle, eles_offset] = renderer::instance()->element_allocator().alloc_buffer(
                s_billboard_indices.size() * sizeof(s_billboard_indices[0])
            );

            /* Upload the data */
            renderer::instance()->element_allocator().buffer_data(
                eles_handle, s_billboard_indices.size() * sizeof(s_billboard_indices[0]), 
                s_billboard_indices.data()
            );
            
            m_elem_handle = eles_handle;
            m_first_index = eles_offset / sizeof(s_billboard_indices[0]);   
            m_element_count = s_billboard_indices.size();
            m_indexed = true;
        }

        ~billboard() override = default;
    
    private:
        static constexpr std::array<mesh::vertex, 4> s_billboard_verices = { ///< Billboard vertices
        mesh::vertex(glm::vec3(-6, -6, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2(1, 1)),
        mesh::vertex(glm::vec3(6, -6, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2(0, 1)),
        mesh::vertex(glm::vec3(6, 6, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2(0, 0)),    
        mesh::vertex(glm::vec3(-6, 6, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2(1, 0))
        };

        static constexpr std::array<unsigned, 6> s_billboard_indices = { ///< Billboard indices
            0, 1, 2,
            2, 3, 0
        };
};
