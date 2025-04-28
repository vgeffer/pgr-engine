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

static const unordered_map<string, GLenum> c_extension_type_map = {
    {".frag", GL_FRAGMENT_SHADER},
    {".vert", GL_VERTEX_SHADER}
};

shader_stage::shader_stage(string path)
    : m_type_bitmask(0) {

    GLint result = GL_FALSE;
    GLenum m_type;
    
    /// @todo [Long-Term]: Shader system overhaul
    
    /* Get type from filename */
    string ext = filesystem::path(path).extension();
    if (auto it = c_extension_type_map.find(ext); it != c_extension_type_map.cend())
        m_type = it->second;
    else throw logic_error("Unknown shader type encountered");

    /* Generate type */
    switch (m_type) {
        case GL_FRAGMENT_SHADER:
            m_type_bitmask |= GL_FRAGMENT_SHADER_BIT;
            break;
        case GL_GEOMETRY_SHADER:
            m_type_bitmask |= GL_GEOMETRY_SHADER_BIT;
            break;
        case GL_VERTEX_SHADER:
            m_type_bitmask |= GL_VERTEX_SHADER_BIT;
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
	GLenum shader = glCreateShader(static_cast<GLenum>(m_type));
	  
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
    m_program = glCreateProgram();
    glProgramParameteri(
        m_program,
        GL_PROGRAM_SEPARABLE, /* Programs are separable, defining a custom pipeline */
        GL_TRUE
    );
    glAttachShader(m_program, shader);

    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &result);
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

    /* Clean up */
    glDetachShader(m_program, shader);
    glDeleteShader(shader);
}

shader_stage::~shader_stage() {

    glDeleteProgram(m_program);
}