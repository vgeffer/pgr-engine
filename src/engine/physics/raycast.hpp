#pragma once
#include "bounding_volumes.hpp"
#include <glm/ext/vector_float3.hpp>

namespace physics {

    struct raycast_result {
        bool result;
        glm::vec3 first_intersection;
        float fi_distance;
    };

    class raycast {

        public:
            raycast_result intersects(bounding_volume& volume);

        private:
            glm::vec3 _origin;
            glm::vec3 _direction;
    };  
};