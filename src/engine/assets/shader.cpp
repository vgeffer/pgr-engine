#include <fcntl.h>
#include <filesystem>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

#include "shader.hpp"
#include "../utils/dynamic_alloc.hpp"

using namespace std;
using namespace utils;
using namespace assets;

static const unordered_map<string, stage_type> _ext_type_map = {
    {".geom", stage_type::SOURCE_GEOM},
    {".frag", stage_type::SOURCE_FRAG},
    {".vert", stage_type::SOURCE_VERT}
};

/* TODO: Multistage linking */
shader_stage::shader_stage(string path)
    : _type_bitmask(0) {

    GLint result = GL_FALSE;
    
    /* TODO: multiple */
    /* TODO: Rewrite loading logic to use C++11 STL */
    /* TODO: Add source resolving for #pragma use*/
    stage_type _type;
    /* Get type from filename */
    string ext = filesystem::path(path).extension();
    if (auto it = _ext_type_map.find(ext); it != _ext_type_map.cend())
        _type = it->second;
    else throw logic_error("Unknown shader type encountered");

    /* Generate type */
    switch (_type) {
        case stage_type::SOURCE_FRAG:
            _type_bitmask |= GL_FRAGMENT_SHADER_BIT;
            break;
        case stage_type::SOURCE_GEOM:
            _type_bitmask |= GL_GEOMETRY_SHADER_BIT;
            break;
        case stage_type::SOURCE_VERT:
            _type_bitmask |= GL_VERTEX_SHADER_BIT;
    }

    /* Check if there already exists cahced version of the shader */
    //if (cache::has_item(path + "_source")) {
    //
    //    _program = glCreateProgram();
    //
    //    /* Load from cache */
    //    size_t shader_size;        
    //    void* shader_data = cache::get_item(path + "_source", &shader_size); 
    //    void* format_data = cache::get_item(path + "_format", NULL); /* Header has a constant size of sizeof(GLenum) */ 
    //
    //    /* Load Data */
    //    glProgramBinary(_program, *(static_cast<GLenum*>(format_data)), shader_data, shader_size);
    //
    //    glGetProgramiv(_program, GL_LINK_STATUS, &result);
    //    if (result != GL_FALSE)
    //        return; /* Shader loaded successfully */
    //
    //    /* If failed, fall through and compile the shader properly */    
    //    glDeleteProgram(_program);
    //}

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
	GLenum shader = glCreateShader(static_cast<GLenum>(_type));
	  
	glShaderSource(shader, 1, reinterpret_cast<const char**>(src_buffer), reinterpret_cast<const GLint*>(&shader_size));
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) 
        throw runtime_error("Shader compilation error"); 
    
    /* Compilation successful, link shader */
    _program = glCreateProgram();
    glProgramParameteri(
        _program,
        GL_PROGRAM_SEPARABLE, /* Programs are separable, defining a custom pipeline */
        GL_TRUE
    );
    glAttachShader(_program, shader);

    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) 
        throw runtime_error("Shader linking error");    
    

    /* Get attribs */
    GLint attrib_count = 0,
          uniform_count = 0;
    glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &attrib_count);
    glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &uniform_count);
    

    GLint longest_attr, longest_uniform;

    /* Buffer to hold attribute names from OpenGL */
    glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &longest_attr);
    dynamic_alloc<char> attr_name = dynamic_alloc<char>(longest_attr);

    /* Buffer to hold uniform names from OpenGL */
    glGetProgramiv(_program,  GL_ACTIVE_UNIFORM_MAX_LENGTH, &longest_uniform);
    dynamic_alloc<char> uniform_name = dynamic_alloc<char>(longest_uniform);

    /* Parse out attribs */
    for (GLint i = 0; i < attrib_count; i++)
    {
        GLint attr_size;
        GLenum attr_type;

        /* Get attrib name */
        glGetActiveAttrib(_program, (GLuint)i, longest_attr, NULL, &attr_size, &attr_type, attr_name);

        /* Reslove location */
        GLint location = glGetAttribLocation(_program, attr_name);

        /* Push to attr map - TODO: maybe save type aswell */
        _used_attrib_locations[string(attr_name)] = location;
    }

    /* Parse out uniforms */
    for (GLint i = 0; i < uniform_count; i++)
    {
        GLint uniform_size;
        GLenum uniform_type;

        /* Get uniform name */
        glGetActiveUniform(_program, (GLuint)i, longest_attr, NULL, &uniform_size, &uniform_type, attr_name);

        /* Reslove location */
        GLint location = glGetUniformLocation(_program, attr_name);

        /* Push to uniform map - TODO: maybe save type aswell */
        _used_uniform_locations[string(attr_name)] = location;
    }


    /* Clean up */
    glDetachShader(_program, shader);
    glDeleteShader(shader);

    /* Linking successful, try caching the shader */
    //GLint program_buffer_length = 0;
    //GLenum program_format = 0;
    //GLsizei program_size = 0;
    //
    //glGetProgramiv(
    //    _program,
    //    GL_PROGRAM_BINARY_LENGTH,
    //    &program_buffer_length
    //);
    //
    //if (program_buffer_length <= 0)
    //    return; /* TODO: Log error, but do not throw */
    //char* program_binary = new char[program_buffer_length];
    //
    //glGetProgramBinary(_program, program_buffer_length, &program_size, &program_format, program_binary);
    //
    //cache::add_item(string(path) + "_source", true, static_cast<void*>(program_binary), program_size);
    //cache::add_item(string(path) + "_format", true, static_cast<void*>(&program_format), sizeof(GLenum));
    //
    //delete[] program_binary;
}

shader_stage::~shader_stage() {

    glDeleteProgram(_program);
} 

GLint shader_stage::attribute_location(string name) const {

    if (auto iter = _used_attrib_locations.find(name); iter != _used_attrib_locations.cend())
        return iter->second;
    return -1;
}

GLint shader_stage::uniform_location(string name) const {

    if (auto iter = _used_uniform_locations.find(name); iter != _used_uniform_locations.cend())
        return iter->second;
    return -1;
}