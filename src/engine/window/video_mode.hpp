#pragma once

#include <glm/ext/vector_int2.hpp>
#include <string>
#include "../../lib/json/json.hpp"
#include <glm/glm.hpp>

class video_mode {

    public:
        enum class window_mode {
            WINDOW, BORDERLESS, FULLSCREEN
        };
        
        enum class aa_level {
            OFF = 0, MSAAx2 = 2, MSAAx4 = 4, MSAAx8 = 8
        };
    
    public:
        constexpr video_mode() 
            : m_w(0), m_h(0), m_win_mode(window_mode::WINDOW), m_antialias_level(aa_level::OFF), m_vsync(false) {}

        constexpr video_mode(int w, int h, window_mode win_mode, aa_level antialias, bool vsync)
            : m_w(w), m_h(h), m_win_mode(win_mode), m_antialias_level(antialias), m_vsync(vsync) {}
        
        video_mode(std::string path, const video_mode& fallback);
        void save(std::string path);

        glm::ivec2 size() const { return glm::ivec2(m_w, m_h); }
        window_mode win_mode() const { return m_win_mode; }
        aa_level antialias_level() const { return m_antialias_level; }
        bool vsync() const { return m_vsync; }

    private:
        int m_w, 
            m_h;
        window_mode m_win_mode;
        aa_level m_antialias_level;
        bool m_vsync;
};

/* Serializer/Deserializer structs */
NLOHMANN_JSON_SERIALIZE_ENUM(video_mode::window_mode, {
    {video_mode::window_mode::WINDOW, "windowed"},
    {video_mode::window_mode::BORDERLESS, "borderless"},
    {video_mode::window_mode::FULLSCREEN, "fullscreen"}
});

NLOHMANN_JSON_SERIALIZE_ENUM(video_mode::aa_level, {
    {video_mode::aa_level::OFF, 0}, {video_mode::aa_level::MSAAx2, 2},
    {video_mode::aa_level::MSAAx4, 4}, {video_mode::aa_level::MSAAx8, 8},
});