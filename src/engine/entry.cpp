///
/// @file entry.cpp
/// @author geffevil
///

#include "runtime.hpp"
#include "window/video_mode.hpp"
#include "game_window.hpp"
#include "utils/global_settings.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace utils;

/**
 * Default video mode to use when config fails to load
 */
const video_mode default_mode = video_mode(1280, 720, window_mode::WINDOW, aa_level::OFF, false);


/**
 * @brief Entry point
 * Main function
 */
int main(int argc, char** argv) {
        
    /* Parse cmd args */
    for (int i = 0; i < argc; i++) {
            
    }
    
    try {
        utils::global_settings global_settings("data/project.conf");
        video_mode vid_mode = video_mode("video.conf", default_mode);
        game_window window = game_window("Test-Title", &vid_mode);
        engine_runtime runtime = engine_runtime(&window);
        
        runtime.start();

    } catch (runtime_error& e) {

        cerr << e.what() << endl;
        return -1;   
    }

    return 0;
}
