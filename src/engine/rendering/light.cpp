#include "light.hpp"
#include "renderer.hpp"
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


using namespace rendering;
using namespace rendering::lights;

REGISTER_COMPONENT(directional_light);
REGISTER_COMPONENT(spot_light);
REGISTER_COMPONENT(point_light);

light::light(scene::scene_node* parent, light_type type, glm::vec3 ambient, glm::vec3 diffuse, 
    glm::vec3 specular, float range, float angle)
    : scene::node_component(parent), m_data({
        type,
        ambient, diffuse, specular,
        glm::vec3(0), glm::vec3(0),
        1.0f, 4.5f / range, 75.0f / (range * range),
        angle
    }) {}


void light::prepare_draw(const glm::mat4x4& parent_transform) {
    
    m_data.position = parent_transform * glm::vec4(m_parent->position, 1.0);
    m_data.direction = glm::toMat3(m_parent->rotation) * glm::vec3(0,0,-1);

    renderer::instance()->add_light(m_data);   
}


directional_light::directional_light(scene::scene_node* parent) 
    : light(
        parent, 
        light::light_type::DIRECTIONAL,
        glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        0.0f, 0.0f
    ) {}

directional_light::directional_light(scene::scene_node* parent, const utils::resource& res) 
    : light(
        parent, 
        light::light_type::DIRECTIONAL,
        res.deserialize<glm::vec3>("ambient", glm::vec3(1.0f, 1.0f, 1.0f)),
        res.deserialize<glm::vec3>("diffuse", glm::vec3(1.0f, 1.0f, 1.0f)),
        res.deserialize<glm::vec3>("specular", glm::vec3(1.0f, 1.0f, 1.0f)),
        0.0f, 0.0f
    ) {}


point_light::point_light(scene::scene_node* parent) 
    : light(
        parent, 
        light::light_type::POINT,
        glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        100.0f, 0.0f
    ) {}

point_light::point_light(scene::scene_node* parent, const utils::resource& res) 
    : light(
        parent, 
        light::light_type::POINT,
        res.deserialize<glm::vec3>("ambient", glm::vec3(1.0f, 1.0f, 1.0f)),
        res.deserialize<glm::vec3>("diffuse", glm::vec3(1.0f, 1.0f, 1.0f)),
        res.deserialize<glm::vec3>("specular", glm::vec3(1.0f, 1.0f, 1.0f)),
        res.deserialize<float>("range", 100.0f),
        0.0f
    ) {}

    
spot_light::spot_light(scene::scene_node* parent) 
    : light(
        parent, 
        light::light_type::SPOT,
        glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        100.0f, glm::pi<float>() / 4.0f
    ) {}

spot_light::spot_light(scene::scene_node* parent, const utils::resource& res) 
    : light(
        parent, 
        light::light_type::SPOT,
        res.deserialize<glm::vec3>("ambient", glm::vec3(1.0f, 1.0f, 1.0f)),
        res.deserialize<glm::vec3>("diffuse", glm::vec3(1.0f, 1.0f, 1.0f)),
        res.deserialize<glm::vec3>("specular", glm::vec3(1.0f, 1.0f, 1.0f)),
        res.deserialize<float>("range", 100.0f),
        res.deserialize<float>("angle", glm::pi<float>() / 4.0f)
    ) {}

        