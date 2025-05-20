#include "runtime.hpp"
#include "engine_app.hpp"
#include "game_window.hpp"
#include "utils/exceptions.hpp"
#include "utils/project_settings.hpp"
#include "window/video_mode.hpp"
#include <GLFW/glfw3.h>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>

/// @brief Default video mode used when config fails to load
constexpr video_mode DEFAULT_VIDMODE = video_mode(1280, 720, video_mode::window_mode::WINDOW, video_mode::aa_level::OFF, false);

application::application(const std::string& project_conf) {

    try {        
        m_settings.init(project_conf);

    } catch (std::exception& e) {

        /* Early exit due to app initialization errors */
        std::cerr << "[FATAL ERROR]: " << e.what() << std::endl;
        exit(1);
    }
}

application::exit_status application::run() {

    try {
        video_mode mode = video_mode("video.json", DEFAULT_VIDMODE);
        m_window.create(utils::project_settings::project_name(), mode);
        
        engine_runtime runtime = engine_runtime(m_window);
        runtime.start();
    }
    catch (utils::exceptions::app_reload_message&) {

        return application::exit_status::RELOAD;
    } 
    catch (std::exception& e) {
     
        std::cerr << "[FATAL ERROR]: " << e.what() << std::endl;
        return application::exit_status::FATAL_ERROR;
    }

    m_asset_loader.invalidate();
    return application::exit_status::USER_EXIT;
}
