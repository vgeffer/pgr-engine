#include "video_mode.hpp"
#include <exception>
#include <string>
#include "../utils/resource.hpp"

using namespace std;
using namespace utils;


video_mode::video_mode(string path, const video_mode& fallback) {

    try {
        resource config = resource(path);

        m_w = config.deserialize<int>("video/screen_width");
        m_h = config.deserialize<int>("video/screen_height");
        m_win_mode = config.deserialize<window_mode>("video/win_mode");
        m_antialias_level = config.deserialize<aa_level>("video/aa_level");
        m_vsync = config.deserialize<bool>("video/vsync");

    } catch (exception& e) {

        m_w = fallback.m_w;
        m_h = fallback.m_h;
        m_win_mode = fallback.m_win_mode;
        m_antialias_level = fallback.m_antialias_level;
        m_vsync = fallback.m_vsync;
    }
}

void video_mode::save(std::string path) {
    
    /* Bypass asset loader as this resource may not exist */
    resource config = resource(path);

    config.serialize<int>("video/screen_width", m_w);
    config.serialize<int>("video/screen_height", m_h);
    config.serialize<window_mode>("video/win_mode", m_win_mode);
    config.serialize<aa_level>("video/aa_level", m_antialias_level);
    config.serialize<bool>("video/vsync", m_vsync);
    
    config.save();
}