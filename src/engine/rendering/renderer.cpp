#include "renderer.hpp"
#include "../utils/global_settings.hpp"
#include "../runtime.hpp"
#include "mesh.hpp"
#include <glm/fwd.hpp>
#include <stack>
#include <stdexcept>

using namespace std;
using namespace glm;
using namespace nodes;
using namespace utils;
using namespace assets;
using namespace rendering;


renderer::renderer() {

    if (_instance != nullptr)
        throw logic_error("Renderer already initialised");
    
    for (GLenum capability : global_settings::gl_global_capabilities())
        glEnable(capability);
    
    /* Setup backface culling - this is constant */
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    _instance = this;
    glGenProgramPipelines(1, &_gl_pipeline); /* TODO: mayybe multiple pipelines? */

    /* Generate quad for postprocess */
    const vector<vec2> pp_quad_verts = {
        vec2(-1, 1),
        vec2(-1, -1),
        vec2 (1, 1),
        vec2(1, -1)
    };

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

    glVertexAttribPointer(_get_location("vertices", NULL), 2, GL_FLOAT, false, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    _pp_vertex_stage = new shader_stage("shaders/postprocess.vert");
}

renderer* renderer::instance() {
    return _instance;
}

int renderer::add_postprocess_pass(shader_stage* shader, bool append) {

    if (shader->type_bitmask() != GL_FRAGMENT_SHADER_BIT)
        throw logic_error("Provided shader is not a fragment shader!");

    win_props_t props = engine_runtime::instance()->window()->props();

    postprocess_pass_t pp = {};
    glGenFramebuffers(1, &pp.pp_fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, pp.pp_fbo);
    if (shader->get_uniform_location("screen_texture") >= 0) {

        /* Screen texture is used, create it as texture*/
        pp.pp_use_color_tex = true;
        glGenTextures(1, &pp.pp_color_buffer);
        glBindTexture(GL_TEXTURE_2D, pp.pp_color_buffer);

        /* TODO: Video modes */
        glTexImage2D(
            GL_TEXTURE_2D, 
            0, GL_RGB, 
            props.current_mode->w, 
            props.current_mode->h, 
            0, GL_RGB, 
            GL_UNSIGNED_BYTE, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, global_settings::tex_min_filter());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, global_settings::tex_mag_filter());
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
            props.current_mode->w, 
            props.current_mode->h
        );

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, pp.pp_color_buffer);
    }

    if (shader->get_uniform_location("screen_texture") >= 0) {

        /* Depth texture is used, create it as texture*/
        pp.pp_use_depth_tex = true;
        glGenTextures(1, &pp.pp_depth_buffer);
        glBindTexture(GL_TEXTURE_2D, pp.pp_depth_buffer);

        /* TODO: Video modes */
        glTexImage2D(
            GL_TEXTURE_2D, 
            0, GL_DEPTH24_STENCIL8, 
            props.current_mode->w, 
            props.current_mode->h, 
            0, GL_DEPTH_STENCIL, 
            GL_UNSIGNED_INT_24_8, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, global_settings::tex_min_filter());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, global_settings::tex_mag_filter());
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
            props.current_mode->w, 
            props.current_mode->h
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

void renderer::remove_postprocess_pass(int index) {

    if (index < 0 || index >= _pp_passes.size())
        throw runtime_error("Index out of range!");

    postprocess_pass_t pp = _pp_passes[index];
    _pp_passes.erase(_pp_passes.begin() + index);
    
    /* Delete OGL objects */

    /* Frame buffer */
    glDeleteFramebuffers(1, &pp.pp_fbo);
    
    /* Color buffer */
    if (pp.pp_use_color_tex) glDeleteTextures(1, &pp.pp_color_buffer);
    else glDeleteRenderbuffers(1, &pp.pp_color_buffer);

    /* Depth buffer */
    if (pp.pp_use_depth_tex) glDeleteTextures(1, &pp.pp_depth_buffer);
    else glDeleteRenderbuffers(1, &pp.pp_depth_buffer);
}

void renderer::load_scene(scene_node* root) {

    stack<scene_node*> dfs_stack;
    dfs_stack.push(root);

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
    
    /* Render scene */

    //===============================
    // PASS 1 - Opaque objects
    //===============================
    mesh_render_props* to_draw = nullptr;
    do {
    
        /* Reached Transparent objects, end the pass */
        if (to_draw->material.modulate().a < 1.0f)
            break; 

        /* Bind up matrices & materials */


        /* Draw! */
        to_draw->mesh.draw();        

    } while ((to_draw = to_draw->next()) != nullptr); 

    //===============================
    // PASS 2 - Transparent objects
    //===============================
    do {

        
    } while ((to_draw = to_draw->next()) != nullptr);
    

    //===============================
    // PASS 3 - Post-processing 
    //===============================
    if (camera.render_target() < 0 && !_pp_passes.empty()) {
        int current_pass_idx = 0,
            next_pass_idx = 1;
        
        attach_stage(*_pp_vertex_stage);

        /* Bind Postprocess target -> quad*/
        glBindVertexArray(_pp_quad_vao);

        while (true) {

            if (_pp_passes.size() > next_pass_idx)
                glBindFramebuffer(GL_FRAMEBUFFER, _pp_passes[next_pass_idx].pp_fbo);
            else
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

            attach_stage(*_pp_passes[current_pass_idx].pp_shader);
            if (_pp_passes[current_pass_idx].pp_use_color_tex) set_texture("screen_texture", _pp_passes[current_pass_idx].pp_color_buffer);
            if (_pp_passes[current_pass_idx].pp_use_depth_tex) set_texture("depth_texture", _pp_passes[current_pass_idx].pp_depth_buffer);

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

void renderer::set_uniform(std::string uniform_name, glm::mat3x3& mat) {

    int shader_program;
    int uniform_location = _get_location(uniform_name, NULL);

    glProgramUniformMatrix3fv(
        shader_program, 
        uniform_location, 
        1, GL_FALSE, 
        &mat[0][0]
    );
}

void renderer::set_uniform(std::string uniform_name, glm::mat4x4& mat) {

    int shader_program;
    int uniform_location = _get_location(uniform_name, NULL);
          
    glProgramUniformMatrix4fv(
        shader_program, 
        uniform_location, 
        1, GL_FALSE, 
        &mat[0][0]
    );
}
