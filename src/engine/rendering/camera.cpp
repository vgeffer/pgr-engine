#include "renderer.hpp"
#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "camera.hpp"
#include "../utils/logger.hpp"

#include "../runtime.hpp"

using namespace glm;
using namespace rendering;

constexpr vec3 UP = vec3(0, 1, 0);
constexpr vec3 FORWARD = vec3(0, 0, -1);

REGISTER_COMPONENT(camera);
camera::camera(scene::scene_node* parent, const utils::resource& res)
    : camera(parent, res.deserialize<float>("fov", 70.0f), 
             res.deserialize<float>("near", 0.01f),
             res.deserialize<float>("far", 100.0f),
             res.deserialize<bool>("main", false)) {}
      

camera::camera(scene::scene_node* parent, float fov, float near, float far, bool main) 
    : scene::node_component(parent), m_fov(fov), m_near(near), m_far(far), m_main(main) {

    glCreateBuffers(1, &m_camera_data);
    
    glNamedBufferStorage(m_camera_data, 2 * sizeof(mat4x4) + sizeof(vec4), NULL, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(m_camera_data, 0, sizeof(mat4x4), value_ptr(projection())); /* Push in default projection */
}

camera::~camera() {
    
    glDeleteBuffers(1, &m_camera_data);
}

mat4x4 camera::view() const {

    return lookAt(m_parent->position, m_parent->position + forward(), up());
}

mat4x4 camera::projection() const {
    
    const game_window::window_props_t& props = engine_runtime::instance()->window().props();
    float asp_ratio = static_cast<float>(props.current_mode.size().x) / static_cast<float>(props.current_mode.size().y);

    return perspective(m_fov, asp_ratio, m_near, m_far);
}

vec3 camera::up() const {
    return normalize(toMat3(m_parent->rotation) * UP);
}

vec3 camera::forward() const {
   return normalize(toMat3(m_parent->rotation) * FORWARD);
}

/* Upload projection matrix to OpenGL only when dirty */
float camera::fov(const float& fov) {
    
    m_fov = fov;
    glNamedBufferSubData(m_camera_data, 0, sizeof(mat4x4), value_ptr(projection()));

    return m_fov;
}

float camera::near(const float& near) {
        
    m_near = near;
    glNamedBufferSubData(m_camera_data, 0, sizeof(mat4x4), value_ptr(projection())); /* Push in default projection */

    return m_near;
}

float camera::far(const float& far) {

    m_far = far;
    glNamedBufferSubData(m_camera_data, 0, sizeof(mat4x4), value_ptr(projection())); /* Push in default projection */

    return m_far;
}

void camera::make_active() {

    if (!m_parent->in_active_scene()) {
        logger::error << "Unable to make camera active, not in scene!" << std::endl;
        return;
    }
    
    renderer::instance()->set_active_camera(m_parent->component<camera>());
}

void camera::scene_enter() {
    
    if (m_main) make_active();
}

void camera::prepare_draw(const mat4x4& parent_transform) {

    vec4 world_pos = vec4(m_parent->position, 1.0) * parent_transform;

    glNamedBufferSubData(
        m_camera_data, 
        2 * sizeof(mat4x4), sizeof(vec4), 
        value_ptr(world_pos)
    );
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