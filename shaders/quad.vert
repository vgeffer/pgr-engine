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

vec3 vertex(uint idx) {
    return vec3(
        b_vertices[idx].pos[0],
        b_vertices[idx].pos[1],
        b_vertices[idx].pos[2]
    );
}

vec2 uv(uint idx) {
    return vec2(
        b_vertices[idx].uv[0],
        b_vertices[idx].uv[1]
    );
}

out vec2 o_uv;

void main() {

    o_uv = uv(gl_VertexID);
    gl_Position = vec4(vertex(gl_VertexID), 1.0);
}   