#include <cmath>
#include <glm/fwd.hpp>
#include "camera.hpp"
#include "scene_node.hpp"
#include "../runtime.hpp"

using namespace glm;
using namespace nodes;

static inline mat4x4 _make_projection(float fov, float near, float far) {
    win_props_t& win = engine_runtime::instance()->window()->props();

    float tan_fov = tanf(glm::radians(fov / 2));
    float asp_ratio = (float)win.current_mode->w / (float)win.current_mode->h;

    return mat4x4(
        1.0f / tan_fov, 0, 0, 0,
        0, 1.0f / (tan_fov * asp_ratio) ,  0, 0,
        0, 0, -((far + near) / (far - near)), -1,
        0, 0, -((2.0f * far * near) / (far - near)), 0
    ); 
}

camera::camera(float fov, float near, float far) 
    : scene_node(), _fov(fov), _near(near), _far(far), _projection(_make_projection(fov, near, far)) {}


mat4x4 camera::view() const {

    /* Recompute view */
}

float camera::fov(float& fov) {

    _projection = _make_projection(fov, _near, _far);
    return _fov = fov;
}

float camera::near(float& near) {
    
    _projection = _make_projection(_fov, near, _far);
    return _near = near;
}

float camera::far(float& far) {

    _projection = _make_projection(_fov, _near, far);
    return _far = far;
}

