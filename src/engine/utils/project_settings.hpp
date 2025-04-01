#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace utils {
    
    class project_settings {
        public:
            project_settings(std::string path);
            project_settings(project_settings const&) = delete;
            void operator=(project_settings const&)   = delete;

            static inline std::vector<uint32_t>& gl_global_capabilities() { return  _instance->_gl_global_capabilities; }
            static inline int tex_min_filter() { return _instance->_tex_min_filter; }
            static inline int tex_mag_filter() { return _instance->_tex_mag_filter; }
            static inline float physics_interval() { return _instance->_physics_interval; }   
            

        private:
            inline static project_settings* _instance = nullptr;
            
            /* Rendering */
            std::vector<uint32_t> _gl_global_capabilities;
            
            /* Textures */
            int _tex_min_filter, _tex_mag_filter;

            /* Physics */
            float _physics_interval;
    };
}
    
    
    