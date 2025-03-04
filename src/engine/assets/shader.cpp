#include <fcntl.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

#include "shader.hpp"
#include "../utils/cahce.hpp"


using namespace std;
using namespace assets;

shader_stage::shader_stage(string path) {

    /* Get type from filename */

    GLint result = GL_FALSE;

    /* Check if there already exists cahced version of the shader */
    if (core::cache::has_item(path + "_source")) {

        program = glCreateProgram();

        /* Load from cache */
        size_t shader_size;        
        void* shader_data = core::cache::get_item(path + "_source", &shader_size); 
        void* format_data = core::cache::get_item(path + "_format", NULL); /* Header has a constant size of sizeof(GLenum) */ 

        /* Load Data */
        glProgramBinary(program, *(static_cast<GLenum*>(format_data)), shader_data, shader_size);

        glGetProgramiv(program, GL_LINK_STATUS, &result);
        if (result != GL_FALSE)
            return; /* Shader loaded successfully */

        /* If failed, fall through and compile the shader properly */    
        glDeleteProgram(program);
    }

	int shfd = open(path.c_str(), O_RDONLY);
  
    if (shfd)
        throw runtime_error("Unable to open shader file");

    __off_t shader_size;
    if ((shader_size = lseek(shfd, 0x00, SEEK_END)) < 0)
        throw runtime_error("Unable to read from shader file");
    
    char* src_buffer = new char[shader_size];
	if (read(shfd, src_buffer, shader_size) < shader_size) {
		delete[] src_buffer;
		close(shfd);
		throw runtime_error("Unable to read from shader file");
	}

    /* Source read in, close FD */
    close(shfd);

	/* Compile */
	GLenum shader = glCreateShader(static_cast<GLenum>(type));
	  
	glShaderSource(shader, 1, reinterpret_cast<const char**>(src_buffer), reinterpret_cast<const GLint*>(&shader_size));
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) 
        throw runtime_error("Shader compilation error"); 
    
    /* Compilation successful, link shader */
    program = glCreateProgram();
    glProgramParameteri(
        program,
        GL_PROGRAM_SEPARABLE, /* Programs are separable, defining a custom pipeline */
        GL_TRUE
    );
    glAttachShader(program, shader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) 
        throw runtime_error("Shader linking error");    
    
    
    /* Clean up */
    glDetachShader(program, shader);
    glDeleteShader(shader);

    /* Linking successful, try caching the shader */
    GLint program_buffer_length = 0;
    GLenum program_format = 0;
    GLsizei program_size = 0;

    glGetProgramiv(
        program,
        GL_PROGRAM_BINARY_LENGTH,
        &program_buffer_length
    );

    if (program_buffer_length <= 0)
        return; /* TODO: Log error, but do not throw */
    char* program_binary = new char[program_buffer_length];

    glGetProgramBinary(program, program_buffer_length, &program_size, &program_format, program_binary);

    core::cache::add_item(string(path) + "_source", true, static_cast<void*>(program_binary), program_size);
    core::cache::add_item(string(path) + "_format", true, static_cast<void*>(&program_format), sizeof(GLenum));

    delete[] program_binary;
}

shader_stage::~shader_stage() {

    glDeleteProgram(program);
} 