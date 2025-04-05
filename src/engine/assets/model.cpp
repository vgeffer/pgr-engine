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

using namespace glm;
using namespace std;
using namespace assets;
using namespace rendering;

model::model(string path, bool parse_material) 
    : mesh() {    

    /* TODO: Down the line, replace with custom loader */
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(),
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals
    );

    if (!scene || scene->mNumMeshes <= 0)
        throw runtime_error("Failed to load the model " + path);

    /* Grab the 0th mesh */
    const aiMesh* mesh = scene->mMeshes[0];

   /* Bind mesh's VAO */
   glBindVertexArray(attr_buffer);

    vector<mesh::vertex_t> vertices;
    vertices.reserve(mesh->mNumVertices);

    /* Hopefully -O2 will do it's job */
    for (size_t i = 0; i < mesh->mNumVertices; i++)
        vertices.emplace_back(
            glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
            glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
            glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)/* TODO: */
        );
    el_count = vertices.size();
    
    /* Push vertices to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

    if (mesh->HasFaces()) {

        vector<uint32_t> indices;
        indices.reserve(mesh->mNumFaces * 3); /* A reasonable estimate, since all faces are triangles */

        for (size_t i = 0; i < mesh->mNumFaces; i++) {

            const aiFace face = mesh->mFaces[i];
            for (size_t e = 0; e < face.mNumIndices; e++)         
                indices.emplace_back(face.mIndices[e]);   
        }

         /* Indices */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
        el_count = indices.size();
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertices[0]), reinterpret_cast<void*>(0 * sizeof(float)));
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertices[0]), reinterpret_cast<void*>(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertices[0]), reinterpret_cast<void*>(6 * sizeof(float)));
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    /* Unbind anything still bound */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
    
