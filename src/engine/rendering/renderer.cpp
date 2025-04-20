#include "renderer.hpp"
#include "../utils/project_settings.hpp"
#include "../assets/loader.hpp"
#include "../runtime.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int4.hpp>
#include <glm/fwd.hpp>
#include <glm/matrix.hpp>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;
using namespace utils;
using namespace assets;
using namespace rendering;

/* PP update 1: Build depth texture only once */
renderer::renderer() 
    : m_vertex_buffer(gpu_allocator(project_settings::gpu_geometry_buffer_alloc_size())), 
      m_element_buffer(gpu_allocator(project_settings::gpu_geometry_buffer_alloc_size())),
      m_material_buffer(gpu_allocator(project_settings::gpu_material_buffer_alloc_size())),
      m_texture_buffer(gpu_allocator(project_settings::gpu_textures_buffer_alloc_size())) {

    if (_instance != nullptr)
        throw logic_error("Renderer already initialised");
    
    for (GLenum capability : project_settings::gl_global_capabilities())
        glEnable(capability);

    /* Setup backface culling - this is constant */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LESS);
    glClearColor(0,0,0, 1.0f);

    _instance = this;
    glGenProgramPipelines(1, &m_pipeline);

    /* Generate quad for postprocess */
    const vector<vec2> pp_quad_verts = {
        vec2(-1, 1),
        vec2(-1, -1),
        vec2 (1, 1),
        vec2(1, -1)
    };

    /* Load shader */
    m_postprocess_vertex_shader = loader::load<shader_stage>("shaders/postprocess.vert", loader::caching_policy::KEEPALIVE);

    /* Create pp_quad */
    glCreateVertexArrays(1, &m_pp_quad_vao);
    glCreateBuffers(1, &m_pp_quad_vbo);
    glNamedBufferData(
        m_pp_quad_vbo, 
        pp_quad_verts.size() * sizeof(pp_quad_verts[0]), 
        pp_quad_verts.data(), 
        GL_STATIC_DRAW
    );

    /* Create model vao with ebo */
    glCreateVertexArrays(1, &m_models_vao);

    /* Bind EBO and VBO */
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO, m_vertex_buffer.buffer());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POSTPROCESS_VERTEX_SSBO, m_pp_quad_vbo);
    glVertexArrayElementBuffer(m_models_vao, m_element_buffer.buffer());

    /* Bind Texture and Model budder */
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MATERIAL_SSBO, m_material_buffer.buffer());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, TEXTURE_SSBO, m_texture_buffer.buffer());

    /* Create Draw command queue */
    glCreateBuffers(1, &m_draw_cmd_queue);

    /* Create dynamic object data storage */
    glCreateBuffers(1, &m_object_storage);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, OBJECT_SSBO, m_object_storage);

    /* Create light buffer */
    glCreateBuffers(1, &m_light_storage);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, LIGHTS_SSBO, m_light_storage);

    /* Create global depth RBO for postprocessing */
    glCreateRenderbuffers(1, &m_pass_depth_attachment);
    //win_props_t props = engine_runtime::instance()->window().props();
//
    //glNamedRenderbufferStorage(
    //    m_pass_depth_attachment, GL_DEPTH24_STENCIL8,
    //    props.current_mode.w, props.current_mode.h
    //);

    /* Create global depth texture */
}

renderer::~renderer() {

    for (auto& [bitmask, stage] : m_attached_shader_stages)
        glUseProgramStages(m_pipeline, bitmask, 0);

    glDeleteProgramPipelines(1, &m_pipeline);

    /* Delete postprocess passes */
    for (size_t i = 0; i < m_passes.size(); i++)
        remove_postprocess_pass(i);

    glDeleteBuffers(1, &m_pp_quad_vbo);
    glDeleteVertexArrays(1, &m_pp_quad_vbo);

    /* Delete instance data */
    _instance = nullptr; 
}

size_t renderer::add_postprocess_pass(shader_stage* shader, bool append) {

    if (shader->type_bitmask() != GL_FRAGMENT_SHADER_BIT)
        throw logic_error("Provided shader is not a fragment shader!");

    const game_window::window_props_t& props = engine_runtime::instance()->window().props();
    GLuint pass_fbo, pass_color_texture;

    glCreateFramebuffers(1, &pass_fbo);
    glCreateTextures(GL_TEXTURE_2D, 1, &pass_color_texture);

    glTextureStorage2D(
        pass_color_texture, 1, GL_RGB8, 
        props.current_mode.size().x, props.current_mode.size().y
    );

    glTextureParameteri(pass_color_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(pass_color_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(pass_color_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(pass_color_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    /* Attach */
    glNamedFramebufferTexture(pass_fbo, GL_COLOR_ATTACHMENT0, pass_color_texture, 0);
    glNamedFramebufferRenderbuffer(pass_fbo, GL_DEPTH_ATTACHMENT,  GL_RENDERBUFFER, m_pass_depth_attachment);

    /* Check if framebuffer is complete */
    if (glCheckNamedFramebufferStatus(pass_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw runtime_error("Framebuffer not complete yet!");

    if (append)
        m_passes.emplace(m_passes.end(), m_postprocess_pass{pass_fbo, pass_color_texture, true});

    m_passes.emplace(m_passes.begin(), m_postprocess_pass{pass_fbo, pass_color_texture, true});
    return 0;
}

void renderer::remove_postprocess_pass(size_t index) {

    m_postprocess_pass& pass = m_passes.at(index);
    
    glDeleteFramebuffers(1, &pass.m_fbo);
    glDeleteTextures(1, &pass.m_color_texture);

    m_passes.erase(m_passes.begin() + index);
}

void renderer::set_active_camera(const utils::observer_ptr<camera>& camera) {

    if (!camera.valid() || m_active_camera == camera)
        return;

    m_active_camera = camera;
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_UBO, m_active_camera->camera_data());
} 

void renderer::request_draw(const observer_ptr<mesh_instance>& mesh, glm::mat4x4 transform) {

    /* If mesh is invalid, there is no point in drawing it */
    if (!mesh.valid())
        return;









    object_data data =  {
        transform,
        glm::transpose(glm::inverse(transform)),
        glm::identity<mat3x3>(),
        mesh->get_material().material_index()
    };

    /// @todo [Short-Term]: Object grouping. 
    /// @todo [Very-Short-Term]: Also pass shading info
}

/* This... this is gonna be a big one */
void renderer::draw_scene() {

    /// @todo [Long-Term]: Implement render textures & deferred shading
    
    /* Clear buffer */
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    /* No valid camera bound, end the draw function */
    if (!m_active_camera.valid())
        return;

    /* Update view matrix uniform */
    glNamedBufferSubData(
        m_active_camera->camera_data(), 
        sizeof(mat4x4), sizeof(mat4x4), 
        glm::value_ptr(m_active_camera->view())
    );

    /* Create command queues */
    std::vector<m_draw_command_elements> commands;
    std::vector<m_object_data> object_data;
    
    commands.reserve(m_enqueued_draws.size());
    object_data.reserve(m_enqueued_draws.size());
    
    for (auto [mesh, data] : m_enqueued_draws) {

        /* Prepare transform and command buffers */
        m_draw_command_elements command = {
            mesh->element_count(),
            1, /* No instancing RN */
            mesh->first_index(),
            static_cast<int>(mesh->first_vertex()),
            0 /* No instancing RN */
        };

        commands.push_back(command);
        object_data.push_back(data);
    }

    
    /* Update light data */
    /// @todo [Long-Term]: Update lights only when dirty
    glNamedBufferData(m_light_storage, m_lights.size() * sizeof(m_lights[0]), m_lights.data(), GL_DYNAMIC_DRAW);

    glNamedBufferData(m_draw_cmd_queue, commands.size() * sizeof(commands[0]), commands.data(), GL_DYNAMIC_DRAW);
    glNamedBufferData(m_object_storage, object_data.size() * sizeof(object_data[0]), object_data.data(), GL_DYNAMIC_DRAW);

    /* Only TWO (engine) uniforms per shader! */
    set_uniform("draw_id_offset", 0u);
    set_uniform("light_count", static_cast<uint>(m_lights.size()));

    glBindProgramPipeline(m_pipeline);
    glBindVertexArray(m_models_vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_draw_cmd_queue);


    //===============================
    // PASS 1 - Opaque objects
    //===============================
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, commands.size(), 0);

    //===============================
    // PASS 2 - Transparent objects
    //===============================


    //glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, 1, 0);


    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glBindVertexArray(0);
    glBindProgramPipeline(0);

    
    //===============================
    // PASS 3 - Post-processing 
    //===============================


    size_t last_frame_lights = m_lights.size();
    m_lights.clear();
    m_lights.reserve(last_frame_lights);
   
    m_enqueued_draws.clear();
    m_enqueued_draws.reserve(commands.size());
}

vector<pair<GLuint, GLint>> renderer::attribute_location(string name) const {

    vector<pair<GLuint, GLint>> found_locations;

    for (auto [type, stage] : m_attached_shader_stages) {

        GLint location;
        if ((location = stage->attribute_location(name)) >= 0)
            found_locations.push_back(make_pair(*stage, location));
    }

    return found_locations;
}

vector<pair<GLuint, GLint>> renderer::uniform_location(string name) const {

    vector<pair<GLuint, GLint>> found_locations;

    for (auto [type, stage] : m_attached_shader_stages) {

        GLint location;
        if ((location = stage->uniform_location(name)) >= 0)
            found_locations.push_back(make_pair(*stage, location));
    }

    return found_locations;
}

template <typename T> 
void renderer::set_uniform(std::string uniform_name, const T& val) {

    auto found_locations = uniform_location(uniform_name);
    if (found_locations.size() <= 0)
        return;

    for (auto [program, location] : found_locations)
        _set_uniform_value<T>(program, location, val);
}

void renderer::_attach_stage(shared_ptr<shader_stage>& stage) {

    /* Grab iterator */
    auto iter = m_attached_shader_stages.find(stage->type_bitmask());

    if (iter == m_attached_shader_stages.end()) {

        m_attached_shader_stages.emplace(stage->type_bitmask(), stage);
        glUseProgramStages(m_pipeline, stage->type_bitmask(), *stage);
        return;
    }

    /* Optimize shader changes */
    if (m_attached_shader_stages[iter->first] == stage)
        return;
    
    m_attached_shader_stages[iter->first] = stage;
    glUseProgramStages(m_pipeline, stage->type_bitmask(), *stage);
}


/* Uniform setters - there is a lot of them*/
template <> void renderer::_set_uniform_value<int>(GLuint program, GLint location, const int& val) {
    glProgramUniform1i(program, location, val);
}

template <> void renderer::_set_uniform_value<uint>(GLuint program, GLint location, const uint& val) {
    glProgramUniform1ui(program, location, val);
}

template <> void renderer::_set_uniform_value<float>(GLuint program, GLint location, const float& val) {
    glProgramUniform1f(program, location, val);
}

template <> void renderer::_set_uniform_value<bool>(GLuint program, GLint location, const bool& val) {
    glProgramUniform1i(program, location, val);
}

template <> void renderer::_set_uniform_value<ivec2>(GLuint program, GLint location, const ivec2& val) { 
    glProgramUniform2iv(program, location, 1, value_ptr(val)); 
}

template <> void renderer::_set_uniform_value<vec2>(GLuint program, GLint location, const vec2& val) {
        glProgramUniform2fv(program, location, 1, value_ptr(val));
}

template <> void renderer::_set_uniform_value<mat2x2>(GLuint program, GLint location, const mat2x2& val) {
    glProgramUniformMatrix2fv(program, location, 1, GL_FALSE, value_ptr(val));
}

template <> void renderer::_set_uniform_value<ivec3>(GLuint program, GLint location, const ivec3& val) {
    glProgramUniform3iv(program, location, 1,  value_ptr(val));
}

template <> void renderer::_set_uniform_value<vec3>(GLuint program, GLint location, const vec3& val) {
    glProgramUniform3fv(program, location, 1,  value_ptr(val));
}

template <> void renderer::_set_uniform_value<mat3x3>(GLuint program, GLint location, const mat3x3& val) {

    glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, value_ptr(val));
}

template <> void renderer::_set_uniform_value<ivec4>(GLuint program, GLint location, const ivec4& val) {
    glProgramUniform2iv(program, location, 1, value_ptr(val));
}

template <> void renderer::_set_uniform_value<vec4>(GLuint program, GLint location, const vec4& val) {
    glProgramUniform4fv(program, location, 1, value_ptr(val));
}

template <> void renderer::_set_uniform_value<mat4x4>(GLuint program, GLint location, const mat4x4& val) {
    glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, value_ptr(val));
}
