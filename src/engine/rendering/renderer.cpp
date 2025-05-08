#include "renderer.hpp"

#include <stdexcept>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "mesh.hpp"
#include "meshes/quad.hpp"
#include "meshes/skybox.hpp"
#include "../utils/project_settings.hpp"
#include "../runtime.hpp"
#include "../assets/loader.hpp"


using namespace std;
using namespace glm;
using namespace utils;
using namespace assets;
using namespace rendering;

/// @brief list of active attachments for opaque passes 
constexpr std::array<GLenum, 1> g_opaque_attachments = { GL_COLOR_ATTACHMENT0 };

/// @brief list of active attachments for OIT
constexpr std::array<GLenum, 2> g_transparent_attachments = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

renderer::renderer() 
    : m_vertex_buffer(gpu_allocator(project_settings::gpu_geometry_buffer_alloc_size())), 
      m_element_buffer(gpu_allocator(project_settings::gpu_geometry_buffer_alloc_size())),
      m_material_buffer(gpu_allocator(project_settings::gpu_material_buffer_alloc_size())),
      m_texture_buffer(gpu_allocator(project_settings::gpu_textures_buffer_alloc_size())) { }

void renderer::init() {

    if (s_instance != nullptr)
        throw logic_error("Renderer already initialised");
    
    for (GLenum capability : project_settings::gl_global_capabilities())
        glEnable(capability);

    /* Setup backface culling - this is constant */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    s_instance = this;
    glGenProgramPipelines(1, &m_pipeline);

    /* Setup quad */
    auto [q_handle, q_offset] = m_vertex_buffer.alloc_buffer(sizeof(c_quad_mesh));
    m_vertex_buffer.buffer_data(q_handle, sizeof(c_quad_mesh), c_quad_mesh.data());
    m_quad_handle = q_handle;
    m_quad_first_vertex = q_offset / sizeof(mesh::vertex);

    /* Setup skybox mesh */
    auto [s_handle, s_offset] = m_vertex_buffer.alloc_buffer(sizeof(c_skybox_mesh));
    m_vertex_buffer.buffer_data(s_handle, sizeof(c_skybox_mesh), c_skybox_mesh.data());
    m_skybox_handle = s_handle;
    m_skybox_first_vertex = s_offset / sizeof(mesh::vertex);

    /* Load shaders */
    m_quad_vertex_shader = loader::load<shader_stage>("shaders/postprocess.vert");
    m_combination_shader = loader::load<shader_stage>("shaders/combination.frag");
    m_skybox_vertex_shader = loader::load<shader_stage>("shaders/skybox.vert");
    m_skybox_fragment_shader = loader::load<shader_stage>("shaders/skybox.frag");
    

    for (const auto& stage_path : project_settings::default_shaders()) {

        std::shared_ptr<shader_stage> stage = loader::load<shader_stage>(stage_path);
        if (m_default_shaders.find(stage->type_bitmask()) != m_default_shaders.end()) {
            std::cerr << "[WARNING] A shader type " << stage->type_bitmask() << " already has a shader attached! Skipping " << stage_path << std::endl;
            continue;
        }
    
        m_default_shaders.emplace(stage->type_bitmask(), stage);
    }

    /* Create FBOs */
    m_build_fbos();

    /* Create model vao with ebo */
    glCreateVertexArrays(1, &m_models_vao);

    /* Bind EBO and VBO */
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VERTEX_SSBO, m_vertex_buffer.buffer());
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

    /* Create global depth texture */
    m_fog_enabled = false;
    m_fog_color = glm::vec4(0, 0, 0, 1);
    glClearColor(0, 0, 0, 1);
}

renderer::~renderer() {

    for (auto& [type, stage] : m_attached_shader_stages)
        glUseProgramStages(m_pipeline, type, 0);

    glDeleteProgramPipelines(1, &m_pipeline);

    m_vertex_buffer.free_buffer(m_quad_handle);
    m_vertex_buffer.free_buffer(m_skybox_handle);

    /* Destroy FBOs */
    m_destroy_fbos();

    /* Delete instance data */
    s_instance = nullptr; 
}

renderer::pp_pass_handle renderer::add_postprocess_pass(const shared_ptr<shader_stage>& shader) {

    if (shader->type_bitmask() != GL_FRAGMENT_SHADER_BIT)
        throw logic_error("Provided shader is not a fragment shader!");

    return m_postprocess_passes.emplace(m_postprocess_passes.end(), shader);
}

void renderer::remove_postprocess_pass(const renderer::pp_pass_handle& index) {

    if (index == m_postprocess_passes.end()) {
        std::cerr << "[ERROR] Attempting to delete an unknown postprocessing pass" << std::endl;
        return;
    }

    m_postprocess_passes.erase(index);
}

void renderer::set_active_camera(const utils::observer_ptr<camera>& camera) {

    if (!camera.valid() || m_active_camera == camera)
        return;

    m_active_camera = camera;
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_UBO, m_active_camera->camera_data());
} 

void renderer::set_active_skybox(const std::shared_ptr<assets::cubemap>& skybox) {

    m_current_skybox = skybox;
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemap_object());
}

void renderer::request_draw(const observer_ptr<mesh_instance>& mesh_instance, const glm::mat4x4& transform) {

    /* If mesh is invalid, there is no point in drawing it */
    if (!mesh_instance.valid())
        return;

    /* Create draw request */
    draw_request req = {
        draw_request::draw_command{
            mesh_instance->get_mesh()->element_count(),
            1, /* No instancing RN */
            mesh_instance->get_mesh()->first_index(),
            static_cast<int>(mesh_instance->get_mesh()->first_vertex()),
            0 /* No instancing RN */
        },
        draw_request::object_data{
            transform,
            glm::transpose(glm::inverse(transform)),
            glm::identity<mat3x3>(),
            mesh_instance->get_material().material_index()
        },
        mesh_instance->get_material().transparent(),
        mesh_instance->get_material().shader_stages()  
    };

    /* Insert into list */
    m_enqueued_objects.insert(std::move(req), 
        static_cast<GLuint>(*mesh_instance->get_material().shader_stages().at(shader_stage::c_known_stage_types[1])), 
        static_cast<GLuint>(*mesh_instance->get_material().shader_stages().at(shader_stage::c_known_stage_types[1]))
    );
}

/* This... this is gonna be a big one */
void renderer::draw_scene() {

    //===============================
    // SETUP - Prepare rendering
    //===============================

    /* No valid camera bound or nothing to draw, end the draw function */
    if (!m_active_camera.valid() || m_enqueued_objects.empty())
        return;

    /* Update view matrix uniform */
    glNamedBufferSubData(
        m_active_camera->camera_data(), 
        sizeof(mat4x4), sizeof(mat4x4), 
        glm::value_ptr(m_active_camera->view())
    );

    /* Prepare object data for drawing */
    std::vector<render_pass> draw_passes;
    m_prepare_drawing(draw_passes);

    /* Update light data  & prepare for drawing */
    glNamedBufferData(m_light_storage, m_lights.size() * sizeof(m_lights[0]), m_lights.data(), GL_DYNAMIC_DRAW);
    glBindProgramPipeline(m_pipeline);
    glBindVertexArray(m_models_vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_draw_cmd_queue);
 
    GLuint objects_drawn = 0;
    auto pass = draw_passes.begin();

    /* Clean the fbo's depth */
    glClearNamedFramebufferfi(m_default_target.fbo, GL_DEPTH_STENCIL, 0, 1.0f, 0.0f);

    //===============================
    // PASS 1 - Opaque objects
    //===============================

    /* Bind default FBO */
    glBindFramebuffer(GL_FRAMEBUFFER, m_default_target.fbo);

    /* Enable only opaque attachment for drawing and clear it */
    glNamedFramebufferDrawBuffers(m_default_target.fbo, g_opaque_attachments.size(), g_opaque_attachments.data());
    glClearNamedFramebufferfv(m_default_target.fbo, GL_COLOR, 0, value_ptr(glm::vec4(0.0)));

    /* Set appropriate OpenGL state */
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunci(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Itterate over draw passes and draw them */
    for (; pass != draw_passes.end(); ++pass) {
        
        /* Upon reaching the first transparent pass, end this step */
        if (pass->transparent)
            break;

        /* Bind shader delta */
        for (const auto& stage : pass->shader_delta)
            m_attach_stage(stage);

        /* Set uniforms correctly - Only TWO (engine) uniforms per shader! */
        set_uniform("draw_id_offset", objects_drawn, GL_VERTEX_SHADER_BIT);
        set_uniform("light_count", static_cast<uint>(m_lights.size()), GL_FRAGMENT_SHADER_BIT);

        /* Draw! */
        glMultiDrawElementsIndirect(
            GL_TRIANGLES, GL_UNSIGNED_INT, 
            reinterpret_cast<void*>(objects_drawn * sizeof(draw_request::draw_command)), 
            pass->object_count, 0
        );

        objects_drawn += pass->object_count;
    }

    //===============================
    // PASS 2 - Transparent objects
    //===============================

    /* Enable only transparent attachments for drawing and clear them */
    glNamedFramebufferDrawBuffers(m_default_target.fbo, g_transparent_attachments.size(), g_transparent_attachments.data());
    glClearNamedFramebufferfv(m_default_target.fbo, GL_COLOR, 0, value_ptr(glm::vec4(0.0)));
    glClearNamedFramebufferfv(m_default_target.fbo, GL_COLOR, 1, value_ptr(glm::vec4(1.0))); 

    /* Set appropriate OpenGL state */
    glDepthMask(GL_FALSE);
    glBlendFunci(0, GL_ONE, GL_ONE);
    glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

    /* Draw the rest of the passes */
    for (; pass != draw_passes.end(); ++pass) {
    
        /* Bind shader delta */
        for (const auto& stage : pass->shader_delta)
            m_attach_stage(stage);

        /* Set uniforms correctly - Only TWO (engine) uniforms per shader! */
        set_uniform("draw_id_offset", objects_drawn, GL_VERTEX_SHADER_BIT);
        set_uniform("light_count", static_cast<uint>(m_lights.size()), GL_FRAGMENT_SHADER_BIT);

        /* Draw! */
        glMultiDrawElementsIndirect(
            GL_TRIANGLES, GL_UNSIGNED_INT, 
            reinterpret_cast<void*>(objects_drawn * sizeof(draw_request::draw_command)), 
            pass->object_count, 0
        );

        objects_drawn += pass->object_count;
    }

    //===============================
    // INTERMEZZO - Skybox
    //===============================
    
    /* Set opaque attachment up for drawing */
    glNamedFramebufferDrawBuffers(m_default_target.fbo, g_opaque_attachments.size(), g_opaque_attachments.data());
    glDepthFunc(GL_LEQUAL);
    
    /* If the scene has skybox, draw it! */
    if (m_current_skybox) {
        
        m_attach_stage(m_skybox_vertex_shader);
        m_attach_stage(m_skybox_fragment_shader);

        glDrawArraysInstancedBaseInstance(
            GL_TRIANGLES, 
            m_skybox_first_vertex, 
            c_skybox_mesh.size(), 1, 0
        );
    }

    //===============================
    // PASS 3 - Blend pass
    //===============================

    /* If no postprocess passes will follow, choose back buffer directly */
    /* Otherwise, choose the postprocess FBO */
    glBindFramebuffer(
        GL_FRAMEBUFFER, 
        m_postprocess_passes.size() > 0 ? m_postprocess_targets[0].fbo : 0
    );

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_ALWAYS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* If no object nor skybox were drawn, end the frame now */
    if (objects_drawn == 0 && !m_current_skybox) {
        m_end_draw();
        return;
    }
    
    /* Attach default FBO's attachments as textures */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_default_target.opaque_target);   
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_default_target.accum_target);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_default_target.reveal_target);
    
    /* Setup shaders */
    m_attach_stage(m_quad_vertex_shader);
    m_attach_stage(m_combination_shader);
    
    /* Set uniforms */
    set_uniform("opaque_target", 0);
    set_uniform("accum_target", 1);
    set_uniform("reveal_target", 2);
    
    /* Draw! */
    glDrawArraysInstancedBaseInstance(
        GL_TRIANGLES, 
        m_quad_first_vertex, 
        c_quad_mesh.size(), 1, 0
    );
    
    //===============================
    // PASS 4 - Post-processing
    //===============================

    /* Definitions of "front" and "back" buffers - allows having just two buffers for the */
    /* entirety of postprocessing */
    GLuint front = 0, 
           back = 1;

    for (auto pp_pass = m_postprocess_passes.begin(); pp_pass != m_postprocess_passes.end(); ++pp_pass) {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_postprocess_targets[0].color_target);   
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_default_target.depth_stencil_target);
        
        /* Setup shaders */
        m_attach_stage(*pp_pass);
        
        /* Set uniforms */
        set_uniform("color_target", 0);
        set_uniform("depth_target", 1);
        
        /* If this is the last postprocessing pass, bind back buffer */
        glBindFramebuffer(
            GL_FRAMEBUFFER, 
            utils::is_last_in_container(m_postprocess_passes, pp_pass) ? 0 : m_postprocess_targets[0].fbo
        );
        
        /* Draw! */
        glDrawArraysInstancedBaseInstance(
            GL_TRIANGLES, 
            m_quad_first_vertex, 
            c_quad_mesh.size(), 1, 0
        );
        
        /* Swap buffers */
        front = back;
        back ^= 1;
    }

    //===============================
    // END - next-frame preparation
    //===============================
    m_end_draw();
}

template <typename T> 
void renderer::set_uniform(std::string uniform_name, const T& val, GLbitfield stage_hint) {

    for (auto [type, stage] : m_attached_shader_stages) {
        GLint location;

        /* Check if type is being masked by a hint */
        if (!(type & stage_hint))
            continue;
     
        /* Get uniform location - if not found, skip*/
        if ((location  = glGetUniformLocation(static_cast<GLuint>(*stage), uniform_name.c_str())) < 0)
            continue;

        m_set_uniform_value<T>(static_cast<GLuint>(*stage), location, val);
    }
}

void renderer::m_attach_stage(const shared_ptr<shader_stage>& stage) {
    
    m_attached_shader_stages[stage->type_bitmask()] = stage;
    glUseProgramStages(m_pipeline, stage->type_bitmask(), static_cast<GLuint>(*stage));
}

void renderer::m_prepare_drawing(vector<render_pass>& draw_passes) {

    /* Create command queues */
    vector<draw_request::draw_command> commands;
    vector<draw_request::object_data> object_data;

    /* Split objects to opaque and transparent */
    auto first_transparent = stable_partition(
        m_enqueued_objects.begin(), m_enqueued_objects.end(), 
        [](draw_request req) { return !req.transparent; }
    );

    /* Currently bound shaders */
    shader_map current_shaders;
    bool is_transparent = false;
    
    /* This waay before than the code before */
    auto object = m_enqueued_objects.begin();
    while (object != m_enqueued_objects.end()) {
        
        /* Prepare new pass */
        GLuint objects_in_pass = 0;
        shader_list shader_delta;

        /* Prepare space for shaders - by default vert and frag */
        draw_passes.reserve(2); 
        
        /* Process shader delta */
        for (const auto& [type, stage] : object->used_stages) {

            if (auto iter = current_shaders.find(type); iter == current_shaders.end() || iter->second != stage) {
                current_shaders[type] = stage;
                shader_delta.push_back(stage);
            }
        }

        /* Process objects */
        for (; object != m_enqueued_objects.end(); ++object) {

            /* Transparent passes start */
            if (!is_transparent && object == first_transparent) 
                break;

            /* Shader missmatch */
            if (m_has_shader_missmatch(object->used_stages, current_shaders))
                break;

            for (const auto& [key, stage] : object->used_stages) {
                if (auto iter = current_shaders.find(key); iter == current_shaders.end() || iter->second != stage)
                    break;
            }

            /* Push data to queues */
            commands.push_back(std::move(object->command));
            object_data.push_back(std::move(object->data));
            objects_in_pass++;
        }

        /* Save pass */
        if (objects_in_pass > 0)
            draw_passes.emplace_back(render_pass{is_transparent, objects_in_pass, shader_delta});

        if (object == first_transparent)
            is_transparent = true;
    }

    glNamedBufferData(m_draw_cmd_queue, commands.size() * sizeof(commands[0]), commands.data(), GL_DYNAMIC_DRAW);
    glNamedBufferData(m_object_storage, object_data.size() * sizeof(object_data[0]), object_data.data(), GL_DYNAMIC_DRAW);
}

bool renderer::m_has_shader_missmatch(const shader_map& a, const shader_map& b) {

    for (const auto& [type, stage] : a) {

        if (auto iter = b.find(type); iter == b.end() || iter->second != stage)
            return true;
    }

    return false;
}

void renderer::m_end_draw() {
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glBindVertexArray(0);
    glBindProgramPipeline(0);

    size_t last_frame_lights = m_lights.size();
    m_lights.clear();
    m_lights.reserve(last_frame_lights);
    m_enqueued_objects.clear();
}

void renderer::m_build_fbos() {

    const game_window::window_props_t& props = engine_runtime::instance()->window().props();

    /* Opaque attachment */
    glCreateTextures(GL_TEXTURE_2D, 1, &m_default_target.opaque_target);
    glTextureParameteri(m_default_target.opaque_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_default_target.opaque_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(
        m_default_target.opaque_target, 1, GL_RGBA8, 
        props.current_mode.size().x, props.current_mode.size().y
    );

    /* Accum attachment */
    glCreateTextures(GL_TEXTURE_2D, 1, &m_default_target.accum_target);
    glTextureParameteri(m_default_target.accum_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_default_target.accum_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(
        m_default_target.accum_target, 1, GL_RGBA16F, 
        props.current_mode.size().x, props.current_mode.size().y
    );

    /* Reveal attachment */
    glCreateTextures(GL_TEXTURE_2D, 1, &m_default_target.reveal_target);
    glTextureParameteri(m_default_target.reveal_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_default_target.reveal_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(
        m_default_target.reveal_target, 1, GL_R8, 
        props.current_mode.size().x, props.current_mode.size().y
    );

    /* Depth attachment */
    glCreateTextures(GL_TEXTURE_2D, 1, &m_default_target.depth_stencil_target);
    glTextureStorage2D(
        m_default_target.depth_stencil_target, 1, GL_DEPTH24_STENCIL8, 
        props.current_mode.size().x, props.current_mode.size().y
    );

    /* Bind attachments together */
    glCreateFramebuffers(1, &m_default_target.fbo);
    glNamedFramebufferTexture(m_default_target.fbo, GL_COLOR_ATTACHMENT0, m_default_target.opaque_target, 0);
    glNamedFramebufferTexture(m_default_target.fbo, GL_COLOR_ATTACHMENT1, m_default_target.accum_target, 0);
    glNamedFramebufferTexture(m_default_target.fbo, GL_COLOR_ATTACHMENT2, m_default_target.reveal_target, 0);
    glNamedFramebufferTexture(m_default_target.fbo, GL_DEPTH_STENCIL_ATTACHMENT, m_default_target.depth_stencil_target, 0);

    for (auto& pp_target : m_postprocess_targets) {

        glCreateTextures(GL_TEXTURE_2D, 1, &pp_target.color_target);
        glTextureParameteri(pp_target.color_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(pp_target.color_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureStorage2D(
            pp_target.color_target, 1, GL_RGBA8, 
            props.current_mode.size().x, props.current_mode.size().y
        );    

        glCreateFramebuffers(1, &pp_target.fbo);
        glNamedFramebufferTexture(pp_target.fbo, GL_COLOR_ATTACHMENT0, pp_target.color_target, 0);
        glNamedFramebufferDrawBuffer(pp_target.fbo, GL_COLOR_ATTACHMENT0);
    }
}

void renderer::m_destroy_fbos() {


    glDeleteFramebuffers(1, &m_default_target.fbo);
    glDeleteTextures(1, &m_default_target.opaque_target);
    glDeleteTextures(1, &m_default_target.accum_target);
    glDeleteTextures(1, &m_default_target.reveal_target);
    glDeleteTextures(1, &m_default_target.depth_stencil_target);

    for (auto& pp_target : m_postprocess_targets) {

        glDeleteFramebuffers(1, &pp_target.fbo);
        glDeleteTextures(1, &pp_target.color_target);
    }
}

/* Uniform setters - there is a lot of them*/
template <> void renderer::m_set_uniform_value<int>(GLuint program, GLint location, const int& val) {
    glProgramUniform1i(program, location, val);
}

template <> void renderer::m_set_uniform_value<uint>(GLuint program, GLint location, const uint& val) {
    glProgramUniform1ui(program, location, val);
}

template <> void renderer::m_set_uniform_value<float>(GLuint program, GLint location, const float& val) {
    glProgramUniform1f(program, location, val);
}

template <> void renderer::m_set_uniform_value<bool>(GLuint program, GLint location, const bool& val) {
    glProgramUniform1i(program, location, val);
}

template <> void renderer::m_set_uniform_value<ivec2>(GLuint program, GLint location, const ivec2& val) { 
    glProgramUniform2iv(program, location, 1, value_ptr(val)); 
}

template <> void renderer::m_set_uniform_value<vec2>(GLuint program, GLint location, const vec2& val) {
        glProgramUniform2fv(program, location, 1, value_ptr(val));
}

template <> void renderer::m_set_uniform_value<mat2x2>(GLuint program, GLint location, const mat2x2& val) {
    glProgramUniformMatrix2fv(program, location, 1, GL_FALSE, value_ptr(val));
}

template <> void renderer::m_set_uniform_value<ivec3>(GLuint program, GLint location, const ivec3& val) {
    glProgramUniform3iv(program, location, 1,  value_ptr(val));
}

template <> void renderer::m_set_uniform_value<vec3>(GLuint program, GLint location, const vec3& val) {
    glProgramUniform3fv(program, location, 1,  value_ptr(val));
}

template <> void renderer::m_set_uniform_value<mat3x3>(GLuint program, GLint location, const mat3x3& val) {

    glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, value_ptr(val));
}

template <> void renderer::m_set_uniform_value<ivec4>(GLuint program, GLint location, const ivec4& val) {
    glProgramUniform2iv(program, location, 1, value_ptr(val));
}

template <> void renderer::m_set_uniform_value<vec4>(GLuint program, GLint location, const vec4& val) {
    glProgramUniform4fv(program, location, 1, value_ptr(val));
}

template <> void renderer::m_set_uniform_value<mat4x4>(GLuint program, GLint location, const mat4x4& val) {
    glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, value_ptr(val));
}
