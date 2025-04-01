#include "mesh.hpp"
#include "renderer.hpp"

using namespace glm;
using namespace rendering;

mesh::mesh() 
    : draw_mode(GL_TRIANGLES), el_count(0) {

    glGenVertexArrays(1, &attr_buffer);        
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &index_buffer);
}

mesh::~mesh() {

    glDeleteVertexArrays(1, &attr_buffer);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &index_buffer);   
}

mesh_instance::~mesh_instance() {
    
    /* Check if root needs to be adjusted */
    if (renderer::instance()->draw_list() == this) renderer::instance()->draw_list(_next); /* this was head, _next becomes new head */

    /* Re-link list */
    if (_prev) _prev->_next = _next;
    if (_next) _next->_prev = _prev;

    /* Everything else should happen automatically */
}

bool mesh_instance::draw_enqueued() {
    
    bool enqueued = _draw_enqueued;
    _draw_enqueued = false;
    return enqueued;
}

void mesh_instance::request_draw(const mat4x4& transform) {

    _model_mat = transform;
    _draw_enqueued = true;
}

void mesh_instance::on_scene_enter() {
    
}