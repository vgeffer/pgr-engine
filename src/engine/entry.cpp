///
/// @file entry.cpp
/// @author geffevil
///

#include "engine_app.hpp"
#include <string>
#include <string_view>

#define PARSE_ARG(name, type, var) \
    if (std::string_view(argv[arg]) == name && arg < argc - 1) \
        var = type(argv[++arg]);    
    
/// @brief Entry point
///
/// Main function
int main(int argc, char** argv) {    

    /* Defaults */
    std::string project_path = "project.json";

    /* Arg parsing */
    for (int arg = 0; arg < argc; arg++) {
        PARSE_ARG("--project", std::string, project_path);
    }

    application::exit_status status;
    while (true) {
        application app = application(project_path);

        if ((status = app.run()) != application::exit_status::RELOAD)
            break; /* App is not being reloaded, end */
    }

    return static_cast<int>(status);
}
