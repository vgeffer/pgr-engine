#include "video_mode.hpp"
#include <stdexcept>
#include <string>
#include "../assets/resource.hpp"

using namespace std;
using namespace assets;

video_mode::video_mode(int w, int h, window_mode win_mode, aa_level antialias, bool vsync)
    : w(w), h(h), win_mode(win_mode), antialias(antialias), is_vsync(vsync) {}

video_mode::video_mode(string path, const video_mode& fallback) {

    try {
        resource config = *asset::load<resource>(path, asset::caching_policy::NO_CACHE);

        w = config.deserialize<int>("video/screen_width");
        h = config.deserialize<int>("video/screen_height");
        win_mode = config.deserialize<window_mode>("video/win_mode");
        antialias = config.deserialize<aa_level>("video/aa_level");
        is_vsync = config.deserialize<bool>("video/vsync");


    } catch (runtime_error& e) {

        w = fallback.w;
        h = fallback.h;
        win_mode = fallback.win_mode;
        antialias = fallback.antialias;
        is_vsync = fallback.is_vsync;
    }
}

void video_mode::save(std::string path) {
    
    /* Bypass asset loader as this resource may not exist */
    resource config = resource(path);

    config.serialize<int>("video/screen_width", w);
    config.serialize<int>("video/screen_height", h);
    config.serialize<window_mode>("video/win_mode", win_mode);
    config.serialize<aa_level>("video/aa_level", antialias);
    config.serialize<bool>("video/vsync", is_vsync);
    
    config.save();
}