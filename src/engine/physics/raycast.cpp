#include "raycast.hpp"
#include "bounding_volumes.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

using namespace glm;
using namespace physics;
using namespace physics::bounding_volumes;

raycast_result raycast::intersects(bounding_volume& other) {

    switch (other.type()) {
        

        case SPHERE: {
            
            sphere& sph = static_cast<sphere&>(other);
            
            vec3 origin_to_center = sph.center() - _origin;
            float dir_projection = dot(origin_to_center, _direction);

            if (dir_projection < 0 && length(origin_to_center) > sph.radius())
                return {.result = false, .first_intersection = zero<vec3>(), .fi_distance = 0.0f};

            float m = length2(origin_to_center) - dir_projection * dir_projection;

            if (m > sph.radius() * sph.radius())
                return {.result = false, .first_intersection = zero<vec3>(), .fi_distance = 0.0f};

            float q = sqrt(sph.radius() * sph.radius() - m);
            float intersection_dist = (length(origin_to_center) > sph.radius()) ? dir_projection - q : dir_projection + q;
        
            return {.result = true, .first_intersection = _origin + _direction * intersection_dist, .fi_distance = intersection_dist};
        }
    }
    
}