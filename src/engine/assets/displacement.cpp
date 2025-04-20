#include "displacement.hpp"
#include "loader.hpp"
#include <glm/fwd.hpp>
#include <vector>
#include "../utils/resource.hpp"
#include "../utils/resource.hpp"


using namespace glm;
using namespace std;
using namespace assets;
using namespace utils;
using namespace rendering;


displacement::displacement(string path) 
    : mesh() {

    resource displacement_src = resource(path);
    
    /* Parse out the resource */
    _res_horizontal = displacement_src.deserialize<uint32_t>("terrain/width");
    _res_horizontal = displacement_src.deserialize<uint32_t>("terrain/height");
    vector<float> displacement_points = displacement_src.deserialize<vector<float>>("terrain/points");

    /* Load terrain vertex shader */
    _terain_vertex_stage = loader::load<shader_stage>("shaders/terrain.vert", loader::caching_policy::KEEPALIVE);

    /* Enlarge displacements array to res_h * res_v entries */
    int enlarge_by = _res_horizontal * _res_horizontal - displacement_points.size();
    if (enlarge_by > 0) {

        /* Insert directly into pt_displacements... it will most likely be deleted right after upload to OGL */
        vector<float> enlarge_points(enlarge_by, 0.0f);
        displacement_points.insert(displacement_points.end(), enlarge_points.begin(), enlarge_points.end());
    }


    /* Loading */
}