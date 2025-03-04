#include "renderer.hpp"
#include <stdexcept>

using namespace std;
using namespace glm;
using namespace rendering;


renderer::renderer() {

    if (_instance != nullptr)
        throw logic_error("Renderer already initialised");

    _instance = this;
    glGenProgramPipelines(1, &_gl_pipeline); /* TODO: mayybe multiple pipelines? */


}

renderer* renderer::instance() {
    return _instance;
}

void renderer::set_uniform(std::string uniform_name, glm::mat3x3& mat) {

    int shader_program;
    int uniform_location = glGetUniformLocation(
        shader_program, 
        uniform_name.c_str()
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

void renderer::set_uniform(std::string uniform_name, glm::mat4x4& mat) {

    int shader_program;
    int uniform_location = glGetUniformLocation(
        shader_program, 
        uniform_name.c_str()
    );

    /* Most likely not opengl object */
    if (uniform_location < 0) 
        throw std::runtime_error("<ERROR>");
            
    glProgramUniformMatrix4fv(
        shader_program, 
        uniform_location, 
        1, GL_FALSE, 
        &mat[0][0]
    );
}