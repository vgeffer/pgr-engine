#include "terrain_mesh.hpp"
#include "asset.hpp"
#include "resource.hpp"
#include <glm/fwd.hpp>
#include <vector>

using namespace glm;
using namespace std;
using namespace assets;
using namespace rendering;


terrain_mesh::terrain_mesh(string path) 
    : mesh() {


    /* A bit of a hack, but load terrain data as an uncached resource */
    resource displacement = *asset::load<resource>(path, asset::caching_policy::NO_CACHE);
    
    /* Parse out the resource */
    _res_horizontal = displacement.deserialize<uint32_t>("terrain/width");
    _res_horizontal = displacement.deserialize<uint32_t>("terrain/height");
    vector<float> displacement_points = displacement.deserialize<vector<float>>("terrain/points");

    /* Load terrain vertex shader */
    _terain_vertex_stage = asset::load<shader_stage>("shaders/terrain.vert", asset::caching_policy::KEEPALIVE);

    /* Enlarge displacements array to res_h * res_v entries */
    int enlarge_by = _res_horizontal * _res_horizontal - displacement_points.size();
    if (enlarge_by > 0) {

        /* Insert directly into pt_displacements... it will most likely be deleted right after upload to OGL */
        vector<float> enlarge_points(enlarge_by, 0.0f);
        displacement_points.insert(displacement_points.end(), enlarge_points.begin(), enlarge_points.end());
    }

    /* Terrain itself is generated inside the Vertex Shader.  */
    glBindVertexArray(attr_buffer);

    /* Vertices */
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(
        GL_ARRAY_BUFFER, 
        displacement_points.size() * sizeof(displacement_points[0]), 
        displacement_points.data(), 
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(_terain_vertex_stage->attribute_location("vertices"), 1, GL_FLOAT, false, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}