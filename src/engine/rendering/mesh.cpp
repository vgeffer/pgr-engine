#include <memory>
#include <string_view>
#include <utility>
#include "mesh.hpp"
#include "material.hpp"
#include "meshes/billboard.hpp"
#include "meshes/camera.hpp"
#include "renderer.hpp"
#include "../assets/model.hpp"
#include "../assets/displacement.hpp"
#include "../assets/loader.hpp"

using namespace glm;
using namespace rendering;

REGISTER_COMPONENT(mesh_instance);

mesh::mesh() 
    : m_draw_mode(GL_TRIANGLES), m_indexed(false), m_element_count(0), 
      m_first_vertex(0), m_first_index(0) {}

mesh::~mesh() {
    renderer::instance()->vertex_allocator().free_buffer(m_vert_handle);
    
    if (m_indexed)
        renderer::instance()->element_allocator().free_buffer(m_elem_handle);
}

mesh_instance::mesh_instance(scene::scene_node* parent, const utils::resource& res)
    : scene::node_component(parent), m_material(res.deserialize<material>("material", material())) {

    std::string_view type = res.deserialize<std::string_view>("mesh/type");

    /// @todo [Mid-Term] Maybe move to the asset loader? Or known model registry?
    if (type == "model")
        m_mesh = std::move(assets::loader::load<assets::model>(
            res.deserialize<std::string>("mesh/path")
        ));

    else if (type == "displacement")
        m_mesh = std::move(assets::loader::load<assets::displacement>(
            res.deserialize<std::string>("mesh/path")
        ));

    else if (type == "builtin_camera")
        m_mesh = std::make_shared<camera_mesh>();

    else if (type == "billboard")
        m_mesh = std::make_shared<billboard>();

    else throw std::runtime_error("Unknown mesh type encountered!");
}

mesh_instance::mesh_instance(scene::scene_node* parent, std::shared_ptr<mesh>& drawable, const material& mat)
    : scene::node_component(parent), m_mesh(drawable), m_material(mat) {}

void mesh_instance::scene_enter() { 

    /* Enable material */
    m_material.use();
}

void mesh_instance::prepare_draw(const glm::mat4x4& parent_transform) {

    auto mesh = parent()->component<mesh_instance>();
    renderer::instance()->request_draw(mesh, parent_transform);
}