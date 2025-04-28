#include "model.hpp"
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <stdexcept>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "../rendering/renderer.hpp"

using namespace glm;
using namespace std;
using namespace assets;
using namespace rendering;

model::model(string path, bool parse_material) 
    : mesh() {    

    /// @todo [Long-Term]: Down the line, replace with custom loader
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(), 0
        | aiProcess_Triangulate 
        | aiProcess_FlipUVs
        | aiProcess_GenSmoothNormals 
        | aiProcess_CalcTangentSpace 
        | aiProcess_GenBoundingBoxes
        | aiProcess_JoinIdenticalVertices
    );

    if (!scene || scene->mNumMeshes <= 0)
        throw runtime_error("Failed to load the model " + path);

    /* Grab the 0th mesh */
    const aiMesh* mesh = scene->mMeshes[0];

    vector<mesh::vertex> vertices;
    vertices.reserve(mesh->mNumVertices);

    /* Hopefully -O2 will do it's job */
    for (size_t i = 0; i < mesh->mNumVertices; i++)
        vertices.emplace_back(
            glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
            glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
            glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z),
            glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z),
            glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
        );

    /* Reserve buffer for the vertices */
    auto [vert_handle, vert_offset] = renderer::instance()->vertex_allocator().alloc_buffer(vertices.size() * sizeof(vertices[0]));
    m_element_count = vertices.size();

    /* Upload the data */
    renderer::instance()->vertex_allocator().buffer_data(vert_handle, vertices.size() * sizeof(vertices[0]), vertices.data());
    m_vert_handle = vert_handle;
    m_first_vertex = vert_offset / sizeof(vertices[0]);

    if (!mesh->HasFaces())
        throw std::logic_error("Non-indexed meshes not supported yet!");

    m_indexed = true;
    vector<uint32_t> indices;
    indices.reserve(mesh->mNumFaces * 3); /* A reasonable estimate, since all faces are triangles */
    for (size_t i = 0; i < mesh->mNumFaces; i++) {
        const aiFace face = mesh->mFaces[i];
        for (size_t e = 0; e < face.mNumIndices; e++)         
            indices.emplace_back(face.mIndices[e]);   
    }
    
    /* Reserve buffer for the indices */
    auto [elem_handle, elem_offset] = renderer::instance()->element_allocator().alloc_buffer(indices.size() * sizeof(indices[0]));
    m_element_count = indices.size();

    /* Upload the data */
    renderer::instance()->element_allocator().buffer_data(elem_handle, indices.size() * sizeof(indices[0]), indices.data());
    m_elem_handle = elem_handle;
    m_first_index = elem_offset / sizeof(indices[0]);   
}
