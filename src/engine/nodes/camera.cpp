#include <cmath>
#include "camera.hpp"
#include "scene_node.hpp"
#include "../runtime.hpp"

using namespace glm;
using namespace nodes;

camera::camera(float fov, float near, float far) 
    : scene_node(), fov(fov), near(near), far(far) {

    win_props_t& win = engine_runtime::instance()->window()->props();

    float tan_fov = tanf(glm::radians(fov / 2));
    float asp_ratio = (float)win.current_mode->w / (float)win.current_mode->h;

    projection =  mat4x4(
        1.0f / tan_fov, 0, 0, 0,
        0, 1.0f / (tan_fov * asp_ratio) ,  0, 0,
        0, 0, -((far + near) / (far - near)), -1,
        0, 0, -((2.0f * far * near) / (far - near)), 0
    ); 
}

