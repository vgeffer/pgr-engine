#version 460 core

uniform samplerCube u_skybox;

in vec3 i_pos;
layout (location = 0) out vec4 o_color;

void main()
{
    o_color = texture(u_skybox, i_pos);
}
