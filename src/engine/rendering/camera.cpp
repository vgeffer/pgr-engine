#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "camera.hpp"

#include "../runtime.hpp"

using namespace glm;
using namespace rendering;

constexpr vec3 UP = vec3(0, 1, 0);
constexpr vec3 FORWARD = vec3(0, 0, -1);

REGISTER_COMPONENT(camera);
camera::camera(const utils::resource& res)
    : camera(
        res.deserialize<float>("fov", 70.0f),
        res.deserialize<float>("near", 0.01f),
        res.deserialize<float>("far", 100.0f)
    ) {}

camera::camera(float fov, float near, float far) 
    : scene::node_component(), _fov(fov), _near(near), _far(far) {

    glGenBuffers(1, &_matrix_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, _matrix_buffer);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4x4), NULL, GL_STATIC_DRAW); /* Alloc size for two matrices */
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4), sizeof(mat4x4), value_ptr(projection())); /* Push in default projection */
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

camera::~camera() {
    
    glDeleteBuffers(1, &_matrix_buffer);
}

mat4x4 camera::view() const {

    return lookAt(_parent->position(), _parent->position() + forward(), up());
}

mat4x4 camera::projection() const {
    
    win_props_t& win = engine_runtime::instance()->window().props();
    float asp_ratio = (float)win.current_mode.w / (float)win.current_mode.h;

    return perspective(_fov, asp_ratio, _near, _far);
}

vec3 camera::up() const {
    return normalize(toMat3(_parent->rotation()) * UP);
}

vec3 camera::forward() const {
   return normalize(toMat3(_parent->rotation()) * FORWARD);
}

/* Upload projection matrix to OpenGL only when dirty */
float camera::fov(float& fov) {
    
    _fov = fov;
    glBindBuffer(GL_UNIFORM_BUFFER, _matrix_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4), sizeof(mat4x4), value_ptr(projection()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return _fov;
}

float camera::near(float& near) {
    
    
    _near = near;
    glBindBuffer(GL_UNIFORM_BUFFER, _matrix_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4), sizeof(mat4x4), value_ptr(projection()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return _near;
}

float camera::far(float& far) {

    _far = far;
    glBindBuffer(GL_UNIFORM_BUFFER, _matrix_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4), sizeof(mat4x4), value_ptr(projection()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return _far;
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