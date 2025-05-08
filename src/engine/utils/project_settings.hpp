#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

#define CHECK_AND_RETURN(field)                                 \
    if (s_instance == nullptr)                                  \
        throw std::logic_error("Project settings not initialized!"); \
    return s_instance->field;

namespace utils {
    
    /// @todo [Long-Term]: Reflection-based loader
    class project_settings {
        public:

            project_settings();
            void init(std::string path);
            
            static inline const std::string& project_name() { CHECK_AND_RETURN(m_project_name); }
            static inline const std::vector<uint32_t>& gl_global_capabilities() { CHECK_AND_RETURN(m_gl_global_capabilities); }
            static inline size_t gpu_geometry_buffer_alloc_size() { CHECK_AND_RETURN(m_gpu_geometry_buffer_alloc_size); }
            static inline size_t gpu_material_buffer_alloc_size() { CHECK_AND_RETURN(m_gpu_material_buffer_alloc_size); }
            static inline size_t gpu_textures_buffer_alloc_size() { CHECK_AND_RETURN(m_gpu_textures_buffer_alloc_size); }
            static inline int tex_min_filter() { CHECK_AND_RETURN(m_tex_min_filter); }
            static inline int tex_mag_filter() { CHECK_AND_RETURN(m_tex_mag_filter); }
            static inline float physics_interval() { CHECK_AND_RETURN(m_physics_interval); }   
            static inline const std::string& default_scene_path() { CHECK_AND_RETURN(m_default_scene_path); }
            static inline const std::vector<std::string>& default_shaders() { CHECK_AND_RETURN(m_default_shaders); }

        private:
            inline static project_settings* s_instance = nullptr;
            
            /* Project */
            std::string m_project_name;

            /* Rendering */
            std::vector<uint32_t> m_gl_global_capabilities;
            size_t m_gpu_geometry_buffer_alloc_size;
            size_t m_gpu_material_buffer_alloc_size;
            size_t m_gpu_textures_buffer_alloc_size;

            /* Textures */
            int m_tex_min_filter, 
                m_tex_mag_filter;

            /* Physics */
            float m_physics_interval;

            /* Game */
            std::string m_default_scene_path;
            std::vector<std::string> m_default_shaders;
    };
}
    
    
    