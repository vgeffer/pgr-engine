#include "../engine/runtime.hpp"
#include "../engine/window/video_mode.hpp"
#include "../engine/game_window.hpp"
#include <stdexcept>

const video_mode default_mode = video_mode(1280, 720, window_mode::WINDOW, aa_level::OFF, false);

int main(int argc, char** argv) {
        
    /* Parse cmd args */
    for (int i = 0; i < argc; i++) {
            
    }
    
    try {
        video_mode vid_mode = video_mode("video.conf", default_mode);
        game_window window = game_window("Test-Title", &vid_mode);
        engine_runtime runtime = engine_runtime(&window);
        
        runtime.start();

    } catch (std::runtime_error e) {

    }
}
