#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include "../nodes/scene_node.hpp"

using namespace std;
using namespace nodes;

const unordered_map<string, function<scene_node*(string params)> > scene_types = {

    {"graphics::camera", nullptr},
    {"mesh::terrain", nullptr},
};
