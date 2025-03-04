#include "shading_pipeline.hpp"
#include <stdexcept>

shading_pipeline::shading_pipeline() {
    
    glGenProgramPipelines(1, &gl_pipeline);
}

shading_pipeline::~shading_pipeline() {
    
    glDeleteProgramPipelines(1, &gl_pipeline);
}

void shading_pipeline::attach_stage(assets::shader_stage& stage) {
    
    /* Check if already attached */
    //if (loadedShaders.containsKey(stage.getType())) {
    //    if (loadedShaders.get(stage.getType()).getFileName() == null ? stage.getFileName() == null : loadedShaders.get(stage.getType()).getFileName().equals(stage.getFileName())) {
    //        
    //        Logger.log("Skipping " + stage.getFileName() + " - already attached");
    //        return;
    //    } 
    //}
    //
    /* Attach */
    //Logger.log("Attaching shader " + stage.getFileName());
    
    //glUseProgramStages(
    //        gl_pipeline, 
        //        stage.getShaderStageBit(), 
        //        stage.
        //);
        
        //loadedShaders.put(stage.getType(), stage);
}

void shading_pipeline::set_uniform(const char* uniform, glm::mat3x3& mat) {
    
    int shader_program;
    int uniform_location = glGetUniformLocation(
        shader_program, 
        uniform
    );

    /* Most likely not opengl object */
    if (uniform_location < 0) 
        throw std::runtime_error("<ERROR>");
            
    glProgramUniformMatrix3fv(
        shader_program, 
        uniform_location, 
        1, GL_FALSE, 
        &mat[0][0]
    );
}

void shading_pipeline::set_uniform(const char* uniform, glm::mat4x4& mat) {

    int shader_program;
    int uniform_location = glGetUniformLocation(
        shader_program,
        uniform
    );

    if (uniform_location < 0)
        throw std::runtime_error("Error");

    glProgramUniformMatrix4fv(
        shader_program,
        uniform_location,
        1, GL_FALSE,
        &mat[0][0]
    );
}