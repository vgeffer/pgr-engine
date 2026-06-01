#include "application.hpp"
#include "../assets/loader.hpp"
#include "../assets/scene.hpp"
#include "../utils/vulkan/device.hpp"
#include "../window/events.hpp"
#include "../window/window.hpp"
#include "../scene/scene_node.hpp"
#include "../utils/debug.hpp"
#include "app_config.hpp"
#include "../utils/vulkan/swapchain.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <exception>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

using namespace glm;
using namespace std;
using namespace scene;
using namespace utils;
using namespace rendering;
using namespace std::chrono;
using namespace pgreng::app;
using namespace pgreng::window;
using namespace utils::vulkan;

/// @brief Default video mode used when config fails to load
constexpr video_mode DEFAULT_VIDMODE = video_mode(
    1280, 720, 
    video_mode::window_mode::WINDOW, 
    video_mode::aa_level::OFF, 
    false
);

application::application() 
    : m_root_node(new scene_node("GLOBAL_ROOT", scene_node::node_type::ROOT)), m_global_clock(0.0f) {    
    s_instance = this;

    if (!glfwInit())
        throw std::runtime_error("GLFW context initialization failed!");    m_renderer.init();    

    m_vk_instance.add_extensions(app_config::vk_required_extensions());   

    /* TODO: Resolve by CMAKE*/
    debug::init(debug::level::ENGINE_DEBUG, m_vk_instance);
    swapchain::init_instance_support(m_vk_instance);
    m_vk_instance.create();

};

application::~application() {


    glfwTerminate();

    /* Delete the scene */
    delete m_root_node;
}

int application::start() {           

    while (true) {
        
        window::window app_window = window::window();
        m_events.apply_callbacks(app_window);
        glfwSetWindowCloseCallback(app_window.window_props().glfw_handle, [](GLFWwindow*) { application::s_instance->window.close(); });

        /* For now - always choose the best rated device */
        m_vk_instance.select_device(0); 

        /* Setup timekeeping */
        system_clock::time_point tp_prev = system_clock::now(), 
                                 tp_now;

        float physics_delta = 0.0f;
        float physics_interval = app_config::physics_interval();

        /* Load the initial scene */
        auto initial_scene = assets::loader::load<assets::scene_template>(app_config::default_scene_path());
        root_node(initial_scene->instantiate());

        /* Check if renderer has a valid camera */
        if (!m_renderer.has_active_camera())
            cerr << "No main camera found in the scene! For rendering to work, you'll need to set one up manually" << std::endl;

        try {
            /* Mainloop */
            while ( && mainloop_tick());
        } 
        catch (utils::exceptions::window_reload_message) { /* Do nothing */ }
        catch (utils::exceptions::app_reload_message)    { std::rethrow_exception(std::current_exception()); /* Propagate exception one layer up */ }
        catch (std::exception e) {
            debug::error << e.what() << std::endl;
        }
    }
}

scene_node* engine_runtime::root_node(scene_node* root) {

    /* Delete children of root node */
    for (auto& [name, child] : m_root_node->children())
        delete child;

    /* Add node to scene - recursively adds children to scene */
    m_root_node->add_child(root);       

    /* Return */
    return m_root_node;
}

bool mainloop_tick() { 
    m_events.process_frame();

        /* Calculate time elapsed since last frame */
        tp_now = system_clock::now();
        float elapsed = duration_cast<milliseconds>(tp_now - tp_prev).count() / 1000.0f;
        tp_prev = tp_now;        
        
        physics_delta += elapsed;
        m_global_clock += elapsed;

        /* Physics */
        while (physics_delta >= physics_interval) {
            /* Fixed update */
            physics_delta -= physics_interval;
        }

        /* Logic */
        const mat4x4 ident = identity<mat4x4>();
        
        if (m_root_node != nullptr) {
            m_root_node->update_node(elapsed);
            m_root_node->prepare_draw(ident);
        }

        /* Render & postprocess */
        //m_renderer.draw_scene();

        /* Display new frame */
        glfwSwapBuffers(m_window.props().glfw_handle);
    }

    /* Cleanup */
}