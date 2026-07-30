#include "application.hpp"
#include "app_config.hpp"
#include <backends/backend.hpp>
#include "backends/backend/backend_types.hpp"
#include "common/debug.hpp"
#include <chrono>
#include <exception>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

#include <backends/raii.hpp>

using namespace glm;
using namespace std;
using namespace utils;
using namespace pgreng::common;
using namespace std::chrono;
using namespace pgreng::backends;
using namespace pgreng::core::app;


/// @brief Default video mode used when config fails to load
// constexpr video_mode DEFAULT_VIDMODE = video_mode(
//     1280, 720, 
//     video_mode::window_mode::WINDOW, 
//     video_mode::aa_level::OFF, 
//     false
// );

application::application(const std::string& project_path) 
    : m_global_clock(0.0f) {
    s_instance = this;


    m_config.init(project_path);
    m_backend = backends::backend(m_config.backend_name());

    /* Get and parse out backend info */
    backend_metadata info;
    backends::backend::lib_get_info(info);

    debug::info << "Using backend: " << info.name << std::endl;
    //m_vk_instance.add_extensions(app_config::vk_required_extensions());   

    /* TODO: Resolve by CMAKE*/
    //debug::init(debug::level::ENGINE_DEBUG, m_vk_instance);
    //swapchain::init_instance_support(m_vk_instance);
    //m_vk_instance.create();

};

application::~application() {

}

int application::start() {           

    raii::window window = raii::window("title");





    while (true) {

        win_present(window);
    
        /* Setup timekeeping */
        system_clock::time_point tp_prev = system_clock::now();
        float physics_interval = app_config::physics_interval();

        try {
            /* Mainloop */
            while (!win_should_close(window)) {
                
                /* 1. Time keeping*/
                system_clock::time_point tp_now = system_clock::now();
                float elapsed = duration_cast<milliseconds>(tp_now - tp_prev).count() / 1000.0f;
                tp_prev = tp_now;
                m_global_clock += elapsed;

                /* BEGIN MULTITHREAD */
                /* 2. multithreaded update */
                /* 3. physics simulation */
                /* 4. graphics payload preparation */
                /* 5. hand off frame to graphics pipeline */
                /* END MULTITHREAD */

                /* 6. GC mark pass */

                /* 7. Display rendered frame*/
                backends::backend::win_swap_buffers(window);

                /* Graphics pipeline */

                /* 
                    Every GC mark pass (every frame) an epoch counter is increased. If some memory chunk is marked as deleted 
                    and current epoch > delete epoch + frames in flight, free the memory
                */
            }
        } 
        catch (utils::exceptions::window_reload_message) { /* Do nothing */ }
        catch (utils::exceptions::app_reload_message)    { std::rethrow_exception(std::current_exception()); /* Propagate exception one layer up */ }
        catch (std::exception e)                         { debug::error << e.what() << std::endl; }
    }
}
