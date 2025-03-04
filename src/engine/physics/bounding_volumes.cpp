#include "bounding_volumes.hpp"

using namespace glm;
using namespace physics;
using namespace physics::bounding_volumes;

/* Since collisions are symetric, here are some helpers for pairs */
static inline bool _sphere_sphere_intersect(sphere& a, sphere& b) {
    return distance(a.center(), b.center()) < a.radius() + b.radius();
}

static inline bool _sphere_obb_intersect(sphere& a, obb& b) {
    
    #define square(x) (x) * (x)

    float dist_min = 0;
    return dist_min <= square(a.radius()); 
}

static inline bool _sphere_surface_intersect(sphere& a, surface& b) {

}

static inline bool _sat_intersect(bounding_volume& a, bounding_volume& b) {}


sphere::sphere(vec3& center, float radius)
    : bounding_volume(SPHERE), _center(center), _radius(radius) {}

bool sphere::intersects(bounding_volume& other) {

    switch (other.type()) {
        case SPHERE:
            return _sphere_sphere_intersect(*this, static_cast<sphere&>(other));
        
        case OBB:
            return _sphere_obb_intersect(*this, static_cast<obb&>(other));

        case SURFACE:
            return _sphere_surface_intersect(*this, static_cast<surface&>(other));
    }
}

void sphere::transform(mat4x4& mat) {
    return; /* */
}


bool obb::intersects(bounding_volume& other) {

    switch (other.type()) {
        case SPHERE:
            return _sphere_obb_intersect(static_cast<sphere&>(other), *this);
        
        default:
            return _sat_intersect(*this, other);
    }
}





bool surface::intersects(bounding_volume& other) {

    switch (other.type()) {
        case SPHERE:
            return _sphere_surface_intersect(static_cast<sphere&>(other), *this);
        
        default:
            return _sat_intersect(*this, other);
    }
}