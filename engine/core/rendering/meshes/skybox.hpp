#pragma once
#include "../mesh.hpp"
#include <array>

using namespace rendering;
constexpr std::array<rendering::mesh::vertex, 36>  c_skybox_mesh = {
    mesh::vertex(glm::vec3(1, 1, 1), glm::vec3(0, 0, -1), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.5)),
    mesh::vertex(glm::vec3(1, -1, 1), glm::vec3(0, 0, -1), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.75)),
    mesh::vertex(glm::vec3(-1, -1, 1), glm::vec3(0, 0, -1), glm::vec3(0), glm::vec3(0), glm::vec2(0.875, 0.75)),

    mesh::vertex(glm::vec3(1, 1, 1), glm::vec3(0, 0, -1), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.5)),
    mesh::vertex(glm::vec3(-1, -1, 1), glm::vec3(0, 0, -1), glm::vec3(0), glm::vec3(0), glm::vec2(0.875, 0.75)),
    mesh::vertex(glm::vec3(-1, 1, 1), glm::vec3(0, 0, -1), glm::vec3(0), glm::vec3(0), glm::vec2(0.875, 0.5)),

    mesh::vertex(glm::vec3(1, -1, -1), glm::vec3(-0, 1, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.75)),
    mesh::vertex(glm::vec3(-1, -1, -1), glm::vec3(-0, 1, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 1)),
    mesh::vertex(glm::vec3(-1, -1, 1), glm::vec3(-0, 1, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 1)),

    mesh::vertex(glm::vec3(1, -1, -1), glm::vec3(-0, 1, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.75)),
    mesh::vertex(glm::vec3(-1, -1, 1), glm::vec3(-0, 1, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 1)),
    mesh::vertex(glm::vec3(1, -1, 1), glm::vec3(0, 1, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.75)),

    mesh::vertex(glm::vec3(-1, -1, -1), glm::vec3(1, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0)),
    mesh::vertex(glm::vec3(-1, 1, -1), glm::vec3(1, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.25)),
    mesh::vertex(glm::vec3(-1, 1, 1), glm::vec3(1, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.25)),

    mesh::vertex(glm::vec3(-1, -1, -1), glm::vec3(1, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0)),
    mesh::vertex(glm::vec3(-1, 1, 1), glm::vec3(1, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.25)),
    mesh::vertex(glm::vec3(-1, -1, 1), glm::vec3(1, -0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0)),

    mesh::vertex(glm::vec3(-1, 1, -1), glm::vec3(0, -0, 1), glm::vec3(0), glm::vec3(0), glm::vec2(0.125, 0.5)),
    mesh::vertex(glm::vec3(-1, -1, -1), glm::vec3(0, -0, 1), glm::vec3(0), glm::vec3(0), glm::vec2(0.125, 0.75)),
    mesh::vertex(glm::vec3(1, -1, -1), glm::vec3(0, -0, 1), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.75)),

    mesh::vertex(glm::vec3(-1, 1, -1), glm::vec3(0, -0, 1), glm::vec3(0), glm::vec3(0), glm::vec2(0.125, 0.5)),
    mesh::vertex(glm::vec3(1, -1, -1), glm::vec3(0, -0, 1), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.75)),
    mesh::vertex(glm::vec3(1, 1, -1), glm::vec3(0, 0, 1), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.5)),

    mesh::vertex(glm::vec3(1, 1, -1), glm::vec3(-1, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.5)),
    mesh::vertex(glm::vec3(1, -1, -1), glm::vec3(-1, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.75)),
    mesh::vertex(glm::vec3(1, -1, 1), glm::vec3(-1, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.75)),

    mesh::vertex(glm::vec3(1, 1, -1), glm::vec3(-1, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.5)),
    mesh::vertex(glm::vec3(1, -1, 1), glm::vec3(-1, 0, 0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.75)),
    mesh::vertex(glm::vec3(1, 1, 1), glm::vec3(-1, -0, -0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.5)),

    mesh::vertex(glm::vec3(-1, 1, -1), glm::vec3(-0, -1, -0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.25)),
    mesh::vertex(glm::vec3(1, 1, -1), glm::vec3(-0, -1, -0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.5)),
    mesh::vertex(glm::vec3(1, 1, 1), glm::vec3(-0, -1, -0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.5)),

    mesh::vertex(glm::vec3(-1, 1, -1), glm::vec3(-0, -1, -0), glm::vec3(0), glm::vec3(0), glm::vec2(0.375, 0.25)),
    mesh::vertex(glm::vec3(1, 1, 1), glm::vec3(-0, -1, -0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.5)),
    mesh::vertex(glm::vec3(-1, 1, 1), glm::vec3(0, -1, -0), glm::vec3(0), glm::vec3(0), glm::vec2(0.625, 0.25)),
};