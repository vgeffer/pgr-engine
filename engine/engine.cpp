#include "engine.hpp"
#include <backends/raii.hpp>
#include <common/debug.hpp>
#include <core/app/application.hpp>
#include <string>

using namespace pgreng::core;
using namespace pgreng::backends;    

void pgreng::start(const std::string& project_path) {

    int status = -1;
    //Memory

    while(true) {
        try {             
            app::application app(project_path);
            status = app.start();

            /* Everything here is RAII so no cleaning needed */
            exit(status);
        }
        catch (utils::exceptions::app_reload_message) { /* Core app setting have changed, reload */ }
        catch(std::exception e) {

            /* In case the app initialization fails, exit early */
            common::debug::error << "A fatal error occured: " << e.what() << std::endl; 
            exit(1);
        }
    }
}
