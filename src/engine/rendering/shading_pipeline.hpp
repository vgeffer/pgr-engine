#pragma once
#include <glm/glm.hpp>
#include "../assets/shader.hpp"



class shading_pipeline {
    
    public: 
        shading_pipeline();
        ~shading_pipeline();
        void attach_stage (assets::shader_stage& stage);

        void set_uniform(const char* uniform_name, glm::mat3x3& mat);
        void set_uniform(const char* uniform_name, glm::mat4x4& mat);

    private: 
        GLenum gl_pipeline;
        
};
