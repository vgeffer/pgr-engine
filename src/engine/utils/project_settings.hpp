#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace utils {
    
    class project_settings {
        public:
            static void init(std::string path);
            static inline std::vector<uint32_t>& gl_global_capabilities() { return  _instance->_gl_global_capabilities; }
            static inline int tex_min_filter() { return _instance->_tex_min_filter; }
            static inline int tex_mag_filter() { return _instance->_tex_mag_filter; }
            static inline float physics_interval() { return _instance->_physics_interval; }   
            

        private:
            project_settings(std::string path);
            static project_settings* _instance;
            
            /* Rendering */
            std::vector<uint32_t> _gl_global_capabilities;
            
            /* Textures */
            int _tex_min_filter, _tex_mag_filter;

            /* Physics */
            float _physics_interval;


            
    };
}
    
    
    