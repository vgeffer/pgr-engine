#pragma once 

#include "assets/loader.hpp"
#include "game_window.hpp"
#include "utils/project_settings.hpp"

#include <string>

/// @brief Application class - parent of all of the engine components
class application {

    public:

        /// @brief Enum of possible exit codes returned by @c run()
        enum class exit_status {
            RELOAD = -1,    ///< Exit status indicating the whole application should restart (no errors indicated)
            USER_EXIT,      ///< Exit status indicating a normal shutdown of the application (no errors indicated)
            FATAL_ERROR     ///< Exit status indicating a fatal error has occured and application can not continue running
        };

    public:
        /// @brief Constructs the application and all it's components
        /// @param project_conf Filesystem path of the main configuration file
        application(const std::string& project_conf);

        /// @brief Runs the main loop of the application
        /// @returns An exit code indicating action to take
        /// @see exit_status
        exit_status run();

    private:
        game_window m_window;                   ///< Main window
        utils::project_settings m_settings;     ///< Project settings instance
        assets::loader m_asset_loader;          ///< Asset cache instance
};

