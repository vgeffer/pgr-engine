#pragma once 
#include "assets/loader.hpp"
#include "game_window.hpp"
#include "utils/project_settings.hpp"
#include <string>

class application {

    public:
        enum class exit_status {
            RELOAD = -1,
            USER_EXIT,
            FATAL_ERROR
        };

    public:
        application(const std::string& project_conf);
        exit_status run();

    private:
        game_window m_window;
        utils::project_settings m_settings;
        assets::loader m_asset_loader;
};

