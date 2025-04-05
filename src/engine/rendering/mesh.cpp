#include "mesh.hpp"


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

bool mesh_instance::draw_enqueued() {
    
    bool enqueued = _draw_enqueued;
    _draw_enqueued = false;
    return enqueued;
}

void mesh_instance::request_draw(const mat4x4& transform) {

    _model_mat = transform;
    _draw_enqueued = true;
}

void mesh_instance::on_scene_enter() { }