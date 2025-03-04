#include "model.hpp"
#include <glm/glm.hpp>
#include <string>

using namespace glm;
using namespace std;
using namespace assets;

model::model(string path) {    

        
    /* Load the model as Assimp scene */
    AIScene scene = aiImportFile(
            join(getCwd(), "meshes", fileName), 0
    );
    if (scene == null)
        throw new ModelLoadingError("<ASSIMP-LOAD-ERROR>");
    
    /* Since we should import only one mesh... ignore the rest */
    PointerBuffer meshes = scene.mMeshes();
    loadMesh(AIMesh.create(meshes.get(0)));
}


    public void draw() {
        
        glBindVertexArray(meshVAO);
        glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
   }

    /**
     * destroy() - Destroys OGL objects used by the model
     */
    public void destroy() {
        
        /* Free up device memory */
        glDeleteBuffers(meshTexCoordBuffer);
        glDeleteBuffers(meshVertexBuffer);
        glDeleteBuffers(meshIndexBuffer);
        glDeleteBuffers(meshTexBuffer);
        glDeleteVertexArrays(meshVAO);
    }
    
    /**
     * vertices() - Builds an array of vectors from the list of coords
     * @return Array of model's vertices
     */
    public V3f[] vertices() {
         
        V3f[] outVerts = new V3f[this.vertices.size() / 3];
        for (int i = 0; i < this.vertices.size() / 3; i++) 
            outVerts[i] = new V3f(
                    this.vertices.get(i * 3 + 0),
                    this.vertices.get(i * 3 + 1),
                    this.vertices.get(i * 3 + 2)
            );
        
        return outVerts;
    }
    
    /**
     * loadMesh(AIMesh) - Parses the Assimp model object to OGL buffers
     * @param mesh Assimp mesh object
     */
    private void loadMesh(AIMesh mesh) {
        
        /* Init buffers */
        meshTexCoordBuffer = glGenBuffers();
        meshVertexBuffer = glGenBuffers();
        meshIndexBuffer = glGenBuffers();
        meshTexBuffer = glGenBuffers();
        meshVAO = glGenVertexArrays();
        
        
        /* Load vertices */
        vertices = new ArrayList<>();
        AIVector3D.Buffer vertBuffer = mesh.mVertices();
        
        /* Parse to OGL format */
        while (vertBuffer.remaining() > 0) {
            AIVector3D vertex = vertBuffer.get();
            vertices.add(vertex.x());
            vertices.add(vertex.y());
            vertices.add(vertex.z());
        }
    
        /* Load indices */
        List<Integer> indices = new ArrayList<>();
        AIFace.Buffer faceBuffer = mesh.mFaces();
        
        while (faceBuffer.remaining() > 0) {
            
            AIFace face = faceBuffer.get();
            elementsCount += face.mNumIndices();
            
            IntBuffer indBuffer = face.mIndices();
            while (indBuffer.remaining() > 0)
                indices.add(indBuffer.get());            
        }
        
        /* TexCoords */
        List<Float> texCoords = new ArrayList<>();
        AIVector3D.Buffer texCoordBuffer = mesh.mTextureCoords(0);
        
        /* No texrure coords with the model */
        if (texCoordBuffer == null)
            return; /*THROW*/
        
        while (texCoordBuffer.remaining() > 0) {
            
            AIVector3D texCoord = texCoordBuffer.get();
            texCoords.add(texCoord.x());
            texCoords.add(1-texCoord.y());
        }

        /* Assign to OGL objects */
        glBindVertexArray(meshVAO);
        
        /* Vertices */
        glBindBuffer(GL_ARRAY_BUFFER, meshVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, toFloatArray(vertices), GL_STATIC_DRAW); /* It is wildy inefficient... but who cares */
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        /* Texture Coordinates */
        glBindBuffer(GL_ARRAY_BUFFER, meshTexCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, toFloatArray(texCoords), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2,  GL_FLOAT, false, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
                
        /* Indices */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, toIntArray(indices), GL_STATIC_DRAW);

        /* Enable both arrays to be passed to shader */
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);        
        
        /* Unbind anything still bound */
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
