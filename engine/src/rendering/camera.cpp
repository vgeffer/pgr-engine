#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.hpp"
#include "renderer.hpp"
#include "../runtime.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


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
    
    glNamedBufferStorage(m_camera_data, 2 * sizeof(mat4x4) + 2 * sizeof(vec4), NULL, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(m_camera_data, 0, sizeof(mat4x4), value_ptr(projection())); /* Push in default projection */
    glNamedBufferSubData( /* Push clip plane data */
        m_camera_data, 
        2 * sizeof(mat4x4) + sizeof(vec4), sizeof(vec4), 
        value_ptr(glm::vec4(m_near, m_far, 0,0))
    );
}

camera::~camera() {
    
    glDeleteBuffers(1, &m_camera_data);
}

mat4x4 camera::view() const {

    return lookAt(parent()->position, parent()->position + forward(), up());
}

mat4x4 camera::projection() const {
    
    const game_window::window_props_t& props = engine_runtime::instance()->window().props();
    float asp_ratio = static_cast<float>(props.current_mode.size().x) / static_cast<float>(props.current_mode.size().y);

    return perspective(m_fov, asp_ratio, m_near, m_far);
}

const vec3 camera::up() const {
    return normalize(toMat3(parent()->rotation) * UP);
}

const vec3 camera::forward() const {
   return normalize(toMat3(parent()->rotation) * FORWARD);
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
    glNamedBufferSubData( /* Push updated clip plane data */
        m_camera_data, 
        2 * sizeof(mat4x4) + sizeof(vec4), sizeof(vec4), 
        value_ptr(glm::vec4(m_near, m_far, 0,0))
    );

    return m_near;
}

float camera::far(const float& far) {

    m_far = far;
    glNamedBufferSubData(m_camera_data, 0, sizeof(mat4x4), value_ptr(projection())); /* Push in default projection */
    glNamedBufferSubData( /* Push updated clip plane data */
        m_camera_data, 
        2 * sizeof(mat4x4) + sizeof(vec4), sizeof(vec4), 
        value_ptr(glm::vec4(m_near, m_far, 0,0))
    );

    return m_far;
}

void camera::make_active() {

    if (!parent()->in_active_scene()) {
        std::cerr << "[ERROR] Unable to make camera active, not in scene!" << std::endl;
        return;
    }
    
    renderer::instance()->set_active_camera(parent()->component<camera>());
}

void camera::scene_enter() {
    
    if (m_main) make_active();
}

void camera::prepare_draw(const mat4x4& parent_transform) {

    vec4 world_pos = vec4(parent()->position, 1.0) * parent_transform;

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