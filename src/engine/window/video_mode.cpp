#include "video_mode.hpp"
#include <stdexcept>
#include <string>

#include "../utils/config_parser.hpp"


video_mode::video_mode(int w, int h, window_mode win_mode, aa_level antialias, bool vsync)
    : w(w), h(h), win_mode(win_mode), antialias(antialias), is_vsync(vsync) {}

video_mode::video_mode(std::string path, const video_mode& fallback) {

    try {

        utils::config_parser video_config(path);

        w = video_config.get_value<int>("screen_width");
        h = video_config.get_value<int>("screen_height");
        win_mode = window_mode(video_config.get_value<int>("win_mode"));
        antialias = aa_level(video_config.get_value<int>("aa_level"));
        is_vsync = video_config.get_value<bool>("vsync");


    } catch (std::runtime_error& e) {

        w = fallback.w;
        h = fallback.h;
        win_mode = fallback.win_mode;
        antialias = fallback.antialias;
        is_vsync = fallback.is_vsync;
    }
}

void video_mode::save(std::string path) {
    
    utils::config_parser video_config(path, true);

}