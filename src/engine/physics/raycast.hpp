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
            raycast_result inrersect_first(bounding_volume& volume, float max_length = -1);
            

        private:
            glm::vec3 _origin;
            glm::vec3 _direction;
    };  
};