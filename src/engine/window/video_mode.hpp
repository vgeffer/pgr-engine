#pragma once

#include <string>
#include "../../lib/json/json.hpp"

enum class window_mode {
    WINDOW, BORDERLESS, FULLSCREEN
};

enum class aa_level {
    OFF = 0, MSAAx2 = 2, MSAAx4 = 4, MSAAx8 = 8
};

/* Serializer/Deserializer structs */
NLOHMANN_JSON_SERIALIZE_ENUM(window_mode, {
    {window_mode::WINDOW, "windowed"},
    {window_mode::BORDERLESS, "borderless"},
    {window_mode::FULLSCREEN, "fullscreen"}
});

NLOHMANN_JSON_SERIALIZE_ENUM(aa_level, {
    {aa_level::OFF, 0}, {aa_level::MSAAx2, 2},
    {aa_level::MSAAx4, 4},{aa_level::MSAAx8, 8},
});

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