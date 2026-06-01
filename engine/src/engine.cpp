#include "engine.hpp"
#include "common/exceptions.hpp"
#include "app/application.hpp"
#include "utils/debug.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>

using namespace pgreng;
void engine::start(const std::string& app_config_path) {

    while(true) {
        
        try { 
            app::application app;

            int status = app.start();    

            /* Do some last cleanup */
            exit(status);
        }
        catch (utils::exceptions::app_reload_message) { /* Do nothing, everything will auto-reload */ }
        catch(std::exception e) {

            /* In case the app initialization fails, exit early */
            utils::debug::error << "A fatal error occured: " << e.what() << std::endl; 
            exit(1);
        }
    }
}
