#pragma once
#include "../mesh.hpp"
#include <array>

using namespace rendering;
constexpr std::array<mesh::vertex, 6> c_quad_mesh = {
    mesh::vertex(glm::vec3(-1, -1, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2(0, 0)),
    mesh::vertex(glm::vec3(1, -1, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2(1, 0)),
    mesh::vertex(glm::vec3(1, 1, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2(1, 1)),
    mesh::vertex(glm::vec3(1, 1, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2(1, 1)),
    mesh::vertex(glm::vec3(-1, 1, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2(0, 1)),
    mesh::vertex(glm::vec3(-1, -1, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec2(0, 0)),
};
