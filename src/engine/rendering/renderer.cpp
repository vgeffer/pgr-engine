#include "renderer.hpp"
#include "../utils/project_settings.hpp"
#include "../utils/algorithms.hpp"
#include "../runtime.hpp"
#include "mesh.hpp"
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <stdexcept>
#include <stack>
#include <utility>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <iostream>

using namespace std;
using namespace glm;
using namespace nodes;
using namespace utils;
using namespace assets;
using namespace rendering;


renderer::renderer()
    : _draw_list(nullptr) {

    if (_instance != nullptr)
        throw logic_error("Renderer already initialised");
    
    for (GLenum capability : project_settings::gl_global_capabilities())
        glEnable(capability);
    
    /* Setup backface culling - this is constant */
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LESS);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    _instance = this;
    glGenProgramPipelines(1, &_gl_pipeline); /* TODO: mayybe multiple pipelines? */

    /* Get texturing info */
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_max_textures);

    /* Generate quad for postprocess */
    const vector<vec2> pp_quad_verts = {
        vec2(-1, 1),
        vec2(-1, -1),
        vec2 (1, 1),
        vec2(1, -1)
    };

    /* Load shader */
    _pp_vertex_stage = asset::load<shader_stage>("shaders/postprocess.vert", asset::caching_policy::KEEPALIVE);

    /* Assign to OGL objects */
    glBindVertexArray(_pp_quad_vao);
       
    /* Vertices */
    glBindBuffer(GL_ARRAY_BUFFER, _pp_quad_vbo);
    glBufferData(
        GL_ARRAY_BUFFER, 
        pp_quad_verts.size() * sizeof(pp_quad_verts[0]), 
        pp_quad_verts.data(), 
        GL_STATIC_DRAW
    );


    glVertexAttribPointer(_pp_vertex_stage->attribute_location("vertices"), 2, GL_FLOAT, false, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);   
}

renderer::~renderer() {

    for (auto& stage : _attached_shader_stages)
        glUseProgramStages(_gl_pipeline, stage->type_bitmask(), 0);

    glDeleteProgramPipelines(1, &_gl_pipeline);

    /* Delete postprocess passes */
    for (size_t i = 0; i < _pp_passes.size(); i++)
        remove_postprocess_pass(i);

    glDeleteBuffers(1, &_pp_quad_vbo);
    glDeleteVertexArrays(1, &_pp_quad_vao);
}

size_t renderer::add_postprocess_pass(shader_stage* shader, bool append) {

    if (shader->type_bitmask() != GL_FRAGMENT_SHADER_BIT)
        throw logic_error("Provided shader is not a fragment shader!");

    win_props_t props = engine_runtime::instance()->window().props();

    postprocess_pass_t pp = {};
    glGenFramebuffers(1, &pp.pp_fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, pp.pp_fbo);
    if (shader->uniform_location("screen_texture") >= 0) {

        /* Screen texture is used, create it as texture*/
        pp.pp_use_color_tex = true;
        glGenTextures(1, &pp.pp_color_buffer);
        glBindTexture(GL_TEXTURE_2D, pp.pp_color_buffer);

        glTexImage2D(
            GL_TEXTURE_2D, 
            0, GL_RGB, 
            props.current_mode.w, 
            props.current_mode.h, 
            0, GL_RGB, 
            GL_UNSIGNED_BYTE, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, project_settings::tex_min_filter());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, project_settings::tex_mag_filter());
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pp.pp_color_buffer, 0);
    }
    else {

        /* Screen texture is not used, create it as RBO */
        pp.pp_use_color_tex = false;
        glGenRenderbuffers(1, &pp.pp_color_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, pp.pp_color_buffer);
        glRenderbufferStorage(
            GL_RENDERBUFFER, 
            GL_RGB,
            props.current_mode.w, 
            props.current_mode.h
        );

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, pp.pp_color_buffer);
    }

    if (shader->uniform_location("screen_texture") >= 0) {

        /* Depth texture is used, create it as texture*/
        pp.pp_use_depth_tex = true;
        glGenTextures(1, &pp.pp_depth_buffer);
        glBindTexture(GL_TEXTURE_2D, pp.pp_depth_buffer);

        glTexImage2D(
            GL_TEXTURE_2D, 
            0, GL_DEPTH24_STENCIL8, 
            props.current_mode.w, 
            props.current_mode.h, 
            0, GL_DEPTH_STENCIL, 
            GL_UNSIGNED_INT_24_8, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, project_settings::tex_min_filter());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, project_settings::tex_mag_filter());
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, pp.pp_depth_buffer, 0);
    }
    else {

        /* Depth texture is not used, create it as RBO */
        pp.pp_use_depth_tex = false;
        glGenRenderbuffers(1, &pp.pp_depth_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, pp.pp_depth_buffer);
        glRenderbufferStorage(
            GL_RENDERBUFFER, 
            GL_DEPTH24_STENCIL8,
            props.current_mode.w, 
            props.current_mode.h
        );

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pp.pp_depth_buffer);
    }

    /* Check if framebuffer is complete */
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw runtime_error("Framebuffer not complete yet!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (append) {
        
        _pp_passes.push_back(pp);
        return _pp_passes.size() - 1;
    }

    _pp_passes.insert(_pp_passes.begin(), pp);
    return 0;
}

void renderer::remove_postprocess_pass(size_t index) {

    if (index < 0 || index >= _pp_passes.size())
        throw runtime_error("Index out of range!");

    postprocess_pass_t pp = _pp_passes[index];
    _pp_passes.erase(_pp_passes.begin() + index);
    
    /* Frame buffer */
    glDeleteFramebuffers(1, &pp.pp_fbo);
    
    /* Color buffer */
    if (pp.pp_use_color_tex) glDeleteTextures(1, &pp.pp_color_buffer);
    else glDeleteRenderbuffers(1, &pp.pp_color_buffer);

    /* Depth buffer */
    if (pp.pp_use_depth_tex) glDeleteTextures(1, &pp.pp_depth_buffer);
    else glDeleteRenderbuffers(1, &pp.pp_depth_buffer);
}

/* This... this is gonna be a big one */
void renderer::draw_scene(camera& camera) {

    /* Main camera && Postprocess is enabled */
    if (camera.render_target() < 0 && !_pp_passes.empty())
        glBindFramebuffer(GL_FRAMEBUFFER, _pp_passes[0].pp_fbo); /* Bind FBO */

    /* Not main camera, rendering to a texture */
    else if (camera.render_target() >= 0)
        glBindFramebuffer(GL_FRAMEBUFFER, camera.render_target());

    /* Clear buffer */
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    mesh_instance* to_draw = _draw_list;
    GLuint currently_bound_vao = 0;

    //===============================
    // PASS 1 - Opaque objects
    //===============================
    for (; to_draw != nullptr; to_draw = to_draw->next()) {
        
        /* Reached transparent objects, end the pass */
        if (to_draw->mat.modulate().a < 1.0f)
            break; 

        /* Check if model is supposed to be drawn */
        if (!to_draw->draw_enqueued())
            continue; 

        /* Bind up matrices & materials */
        
        /* Binding up */
        _set_uniform("mat_model", to_draw->model_mat());

        /* Draw! */
        const GLuint vao = to_draw->drawable->vao();

        if (vao != currently_bound_vao)
            glBindVertexArray((currently_bound_vao = vao));

        const GLuint mode = to_draw->drawable->mode();    
        if (mode != GL_TRIANGLE_STRIP || mode != GL_TRIANGLE_FAN)
            glDrawElements(mode, to_draw->drawable->element_count(), GL_UNSIGNED_INT, static_cast<void*>(0));
        else glDrawArrays(mode, 0, to_draw->drawable->element_count());
    } 

    //===============================
    // PASS 2 - Transparent objects
    //===============================
    if (to_draw != nullptr) /* Sort only when there is something to sort */
        utils::linklist_sort<mesh_instance>(to_draw, [&](mesh_instance* a, mesh_instance* b) {

            if (!a) return 1;
            if (!b) return -1;
            
            float a_dist2 = length2(camera.position() - reinterpret_cast<scene_node*>(a)->position());
            float b_dist2 = length2(camera.position() - reinterpret_cast<scene_node*>(b)->position());

            return a_dist2 >= b_dist2 ? (a_dist2 > b_dist2 ? -1 : 0) : 1;
        });
    
    for (; to_draw != nullptr; to_draw = to_draw->next()) {
    
        if (!to_draw->draw_enqueued())
            continue;



            /* Binding up */
        _set_uniform("mat_model", to_draw->model_mat());

        /* Draw! */
        const GLuint vao = to_draw->drawable->vao();

        if (vao != currently_bound_vao)
            glBindVertexArray((currently_bound_vao = vao));

        const GLuint mode = to_draw->drawable->mode();    
        if (mode != GL_TRIANGLE_STRIP || mode != GL_TRIANGLE_FAN)
            glDrawElements(mode, to_draw->drawable->element_count(), GL_UNSIGNED_INT, static_cast<void*>(0));
        else glDrawArrays(mode, 0, to_draw->drawable->element_count());
    }

    //===============================
    // PASS 3 - Post-processing 
    //===============================
    if (camera.render_target() < 0 && !_pp_passes.empty()) {
        size_t current_pass_idx = 0,
            next_pass_idx = 1;
        
        _attach_stage(_pp_vertex_stage);

        /* Bind Postprocess target -> quad*/
        glBindVertexArray(_pp_quad_vao);

        while (true) {

            if (_pp_passes.size() > next_pass_idx)
                glBindFramebuffer(GL_FRAMEBUFFER, _pp_passes[next_pass_idx].pp_fbo);
            else
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

            _attach_stage(_pp_passes[current_pass_idx].pp_shader);
            if (_pp_passes[current_pass_idx].pp_use_color_tex) _set_texture("screen_texture", _pp_passes[current_pass_idx].pp_color_buffer);
            if (_pp_passes[current_pass_idx].pp_use_depth_tex) _set_texture("depth_texture", _pp_passes[current_pass_idx].pp_depth_buffer);

            /* Draw */
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            /* Last pass was rendered, end */
            if (_pp_passes.size() <= next_pass_idx)
                break;
            
            current_pass_idx = next_pass_idx;
            next_pass_idx++;
        }

        /* Unbind quad */
        glBindVertexArray(0);
    }
}

vector<pair<GLuint, GLint>> renderer::attribute_location(string name) const {

    vector<pair<GLuint, GLint>> found_locations;

    for (auto& stage : _attached_shader_stages) {

        GLint location;
        if ((location = stage->attribute_location(name)) >= 0)
            found_locations.push_back(make_pair(*stage, location));
    }

    return found_locations;
}

vector<pair<GLuint, GLint>> renderer::uniform_location(string name) const {

    vector<pair<GLuint, GLint>> found_locations;

    for (auto& stage : _attached_shader_stages) {

        GLint location;
        if ((location = stage->uniform_location(name)) >= 0)
            found_locations.push_back(make_pair(*stage, location));
    }

    return found_locations;
}

void renderer::_attach_stage(shared_ptr<shader_stage>& stage) {

    for (int i = 0; i < _attached_shader_stages.size(); i++) {

        /* Stage exists, replace */
        if (_attached_shader_stages[i]->type_bitmask() == stage->type_bitmask()) {
            _attached_shader_stages[i] = stage;
            glUseProgramStages(_gl_pipeline, stage->type_bitmask(), *stage);
            return;
        }
    }

    /* Stage does not exist, add */
    _attached_shader_stages.push_back(stage);
    glUseProgramStages(_gl_pipeline, stage->type_bitmask(), *stage);
}

void _set_texture(std::string texture_name, std::shared_ptr<assets::texture> texture) {

}

void renderer::_set_uniform(std::string uniform_name, int& val) {

    auto found_locations = uniform_location(uniform_name);
    if (found_locations.size() <= 0)
        return;

    for (auto [program, location] : found_locations)
        glProgramUniform1i(
            program, location, 
            val
        );
}

void renderer::_set_uniform(std::string uniform_name, glm::mat3x3& mat) {

    auto found_locations = uniform_location(uniform_name);
    if (found_locations.size() <= 0)
        return;

    for (auto [program, location] : found_locations)
        glProgramUniformMatrix3fv(
            program, location, 
            1, GL_FALSE, 
            &mat[0][0]
        );
}

void renderer::_set_uniform(std::string uniform_name, const glm::mat4x4& mat) {

    auto found_locations = uniform_location(uniform_name);
    if (found_locations.size() <= 0)
        return;

    for (auto [program, location] : found_locations)
        glProgramUniformMatrix4fv(
            program, location, 
            1, GL_FALSE, 
            &mat[0][0]
        );
}
