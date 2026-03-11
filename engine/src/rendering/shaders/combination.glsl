#version 460 core

const float EPSILON = 0.00001f;

uniform sampler2D u_opaque_target;
uniform sampler2D u_accum_target;
uniform sampler2D u_reveal_target;

in vec2 i_uv;
layout (location = 0) out vec4 o_color;


float max_of_components(vec3 v) {
    return max(max(v.x, v.y), v.z);
}

void main()
{
    vec4 opaque = texture(u_opaque_target, i_uv);
    vec4 accum = texture(u_accum_target, i_uv);
    float reveal = texture(u_reveal_target, i_uv).r;

    if (isinf(max_of_components(abs(accum.rgb))))
        accum.rgb = vec3(accum.a);

    o_color = vec4(mix(accum.rgb / max(accum.a, EPSILON), opaque.rgb, reveal), 1.0);
}