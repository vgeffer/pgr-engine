#include <filesystem>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "../utils/buffer.hpp"

using namespace glm;
using namespace std;
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

        std::cerr << "[ERROR] In file " << path << ":\n";
        std::cerr << "  " << error_buffer << std::endl;

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

        std::cerr << "[ERROR] In file " << path << ":\n";
        std::cerr << "  " << error_buffer << std::endl;
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

/* Uniform setters - there is a lot of them*/
template <> void shader_stage::m_set_uniform_value<int>(GLint location, const int& val) {
    glProgramUniform1i(m_program, location, val);
}

template <> void shader_stage::m_set_uniform_value<uint>(GLint location, const uint& val) {
    glProgramUniform1ui(m_program, location, val);
}

template <> void shader_stage::m_set_uniform_value<float>(GLint location, const float& val) {
    glProgramUniform1f(m_program, location, val);
}

template <> void shader_stage::m_set_uniform_value<bool>(GLint location, const bool& val) {
    glProgramUniform1i(m_program, location, val);
}

template <> void shader_stage::m_set_uniform_value<ivec2>(GLint location, const ivec2& val) { 
    glProgramUniform2iv(m_program, location, 1, value_ptr(val)); 
}

template <> void shader_stage::m_set_uniform_value<vec2>(GLint location, const vec2& val) {
    glProgramUniform2fv(m_program, location, 1, value_ptr(val));
}

template <> void shader_stage::m_set_uniform_value<mat2x2>(GLint location, const mat2x2& val) {
    glProgramUniformMatrix2fv(m_program, location, 1, GL_FALSE, value_ptr(val));
}

template <> void shader_stage::m_set_uniform_value<ivec3>(GLint location, const ivec3& val) {
    glProgramUniform3iv(m_program, location, 1,  value_ptr(val));
}

template <> void shader_stage::m_set_uniform_value<vec3>(GLint location, const vec3& val) {
    glProgramUniform3fv(m_program, location, 1,  value_ptr(val));
}

template <> void shader_stage::m_set_uniform_value<mat3x3>(GLint location, const mat3x3& val) {

    glProgramUniformMatrix3fv(m_program, location, 1, GL_FALSE, value_ptr(val));
}

template <> void shader_stage::m_set_uniform_value<ivec4>(GLint location, const ivec4& val) {
    glProgramUniform2iv(m_program, location, 1, value_ptr(val));
}

template <> void shader_stage::m_set_uniform_value<vec4>(GLint location, const vec4& val) {
    glProgramUniform4fv(m_program, location, 1, value_ptr(val));
}

template <> void shader_stage::m_set_uniform_value<mat4x4>(GLint location, const mat4x4& val) {
    glProgramUniformMatrix4fv(m_program, location, 1, GL_FALSE, value_ptr(val));
}
