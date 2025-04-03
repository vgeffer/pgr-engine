#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "camera.hpp"

#include "../runtime.hpp"

using namespace glm;
using namespace nodes;

static inline mat4x4 _make_projection(float fov, float near, float far) {
    
    win_props_t& win = engine_runtime::instance()->window().props();
    float asp_ratio = (float)win.current_mode.w / (float)win.current_mode.h;

    return perspective(fov, asp_ratio, near, far);
}

camera::camera(float fov, float near, float far) 
    : _projection(_make_projection(fov, near, far)), _fov(fov), _near(near), _far(far) {}


mat4x4 camera::view() const {

    vec3 up = normalize(toMat3(rotation()) * vec3(0, 1, 0));
    vec3 fwd = normalize(toMat3(rotation()) * vec3(0, 0, -1));
    return lookAt(position(), vec3(0, 0, -1) + fwd, up);
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

/*


angle = atan2( vector.x, vector.z ) // Note: I expected atan2(z,x) but OP reported success with atan2(x,z) instead! Switch around if you see 90° off.
qx = 0
qy = 1 * sin( angle/2 )
qz = 0
qw = cos( angle/2 )

qx = ax * sin(angle/2)
qy = ay * sin(angle/2)
qz = az * sin(angle/2)
qw = cos(angle/2)
*/