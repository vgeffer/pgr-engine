#include <filesystem>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>


#include "shader.hpp"
#include "../utils/buffer.hpp"
#include "../utils/logger.hpp"

using namespace std;
using namespace utils;
using namespace assets;

static const unordered_map<string, GLenum> _ext_type_map = {
    {".geom", GL_GEOMETRY_SHADER},
    {".frag", GL_FRAGMENT_SHADER},
    {".vert", GL_VERTEX_SHADER}
};

shader_stage::shader_stage(string path)
    : _type_bitmask(0) {

    GLint result = GL_FALSE;
    GLenum _type;
    
    /// @todo [Long-Term]: Shader system overhaul
    
    /* Get type from filename */
    string ext = filesystem::path(path).extension();
    if (auto it = _ext_type_map.find(ext); it != _ext_type_map.cend())
        _type = it->second;
    else throw logic_error("Unknown shader type encountered");

    /* Generate type */
    switch (_type) {
        case GL_FRAGMENT_SHADER:
            _type_bitmask |= GL_FRAGMENT_SHADER_BIT;
            break;
        case GL_GEOMETRY_SHADER:
            _type_bitmask |= GL_GEOMETRY_SHADER_BIT;
            break;
        case GL_VERTEX_SHADER:
            _type_bitmask |= GL_VERTEX_SHADER_BIT;
    }

    ifstream shader_file = ifstream(path, ios::in);

    if (!shader_file.is_open())
        throw runtime_error("Unable to open shader file " + path);
    
    string src_buffer = string(
        istreambuf_iterator<char>(shader_file), 
        istreambuf_iterator<char>()
    );

    if (!shader_file.eof() && shader_file.fail())        
		throw runtime_error("Unable to read from shader file " + path);

    shader_file.close();
    
	/* Compile */
	GLenum shader = glCreateShader(static_cast<GLenum>(_type));
	  
    const char* source = src_buffer.c_str();
	glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)  {
     
        GLint error_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_len);
    
        // The maxLength includes the NULL character
        utils::buffer<GLchar> error_buffer = utils::buffer<GLchar>(error_len);
        glGetShaderInfoLog(shader, error_len, &error_len, error_buffer);

        /// @todo [Mid-Term]: Move to internal logging (Once ready)
        logger::error << "In file " << path << ":\n";
        logger::error << "  " << error_buffer << std::endl;

        /* Output source code of the file*/
        logger::info << " -> Source: \n" << src_buffer.c_str() << std::endl;

        glDeleteShader(shader);
        throw runtime_error("Shader compilation error"); 
    }
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
    if (result == GL_FALSE)  {
     
        GLint error_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_len);
    
        // The maxLength includes the NULL character
        utils::buffer<GLchar> error_buffer = utils::buffer<GLchar>(error_len);
        glGetShaderInfoLog(shader, error_len, &error_len, error_buffer);

        logger::error << "In file " << path << ":\n";
        logger::error << "  " << error_buffer << std::endl;
        glDeleteShader(shader);
        throw runtime_error("Shader linking error"); 
    }   
    
    /// @todo [Mid-Term]: Compact attr/uniform loading into loop
    GLint attrib_count = 0,
          uniform_count = 0,
          uniform_block_count = 0;
    glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &attrib_count);
    glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &uniform_count);
    glGetProgramiv(_program, GL_ACTIVE_UNIFORM_BLOCKS, &uniform_block_count);

    GLint longest_attr, longest_uniform;
    glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &longest_attr);
    glGetProgramiv(_program,  GL_ACTIVE_UNIFORM_MAX_LENGTH, &longest_uniform);
    
    /* Buffer to hold names from OpenGL objects*/
    buffer<char> object_name = buffer<char>(max(longest_attr, longest_uniform));

    /* Parse out attribs */
    for (GLuint i = 0; i < static_cast<GLuint>(attrib_count); i++)
    {
        GLint attr_size;
        GLenum attr_type;

        /* Get attrib name */
        glGetActiveAttrib(_program, i, longest_attr, NULL, &attr_size, &attr_type, object_name);

        /* Reslove location */
        GLint location = glGetAttribLocation(_program, object_name);
        
        /* Skip invalid ones */
        if (location < 0)
            continue;
        
        _used_attrib_locations[string(object_name)] = location;
    }

    /* Parse out uniforms */
    for (GLuint i = 0; i < static_cast<GLuint>(uniform_count); i++)
    {
        /* Get uniform name */
        glGetActiveUniformName(_program, i, longest_uniform, NULL, object_name);

        /* Reslove location */
        GLint location = glGetUniformLocation(_program, object_name);

        /* Skip invalid ones */
        if (location < 0) continue;
        _used_uniform_locations[string(object_name)] = location;
    }

    /* Clean up */
    glDetachShader(_program, shader);
    glDeleteShader(shader);
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
