#version 460 core

struct vertex_t {
    float pos[3];
    float normal[3];
    float tangent[3];
    float bitangent[3];
    float uv[2];
};

layout (std430, binding = 0) restrict readonly buffer vertex_buffer {
    vertex_t b_vertices[];
};


layout (std140, binding = 2) uniform camera {
    mat4x4 u_mat_projection;
    mat4x4 u_mat_view;
    vec4   u_camera_position;
    vec4   u_clip_planes;
};

vec3 vertex(uint idx) {
    return vec3(
        b_vertices[idx].pos[0],
        b_vertices[idx].pos[1],
        b_vertices[idx].pos[2]
    );
}

out vec3 o_pos;

void main() {

    o_pos = vertex(gl_VertexID);
    gl_Position = (u_mat_projection * mat4(mat3(u_mat_view)) * vec4(vertex(gl_VertexID), 1.0)).xyww;
}   