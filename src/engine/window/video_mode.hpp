#pragma once

#include <string>
enum class window_mode {
    WINDOW, BORDERLESS, FULLSCREEN
};

enum class aa_level {
    OFF = 0, MSAAx2 = 2, MSAAx4 = 4, MSAAx8 = 8
};

class video_mode {

    public:
        video_mode(int w, int h, window_mode win_mode, aa_level antialias, bool vsync);
        video_mode(std::string path, const video_mode& fallback);
        void save(std::string path);

        int w, h;
        window_mode win_mode;
        aa_level antialias;
        bool is_vsync;
};