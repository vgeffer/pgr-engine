#include "runtime.hpp"
#include "assets/loader.hpp"
#include "assets/scene.hpp"
#include "events.hpp"
#include "game_window.hpp"
#include "scene/scene_node.hpp"
#include "utils/project_settings.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

using namespace glm;
using namespace std;
using namespace scene;
using namespace utils;
using namespace rendering;
using namespace std::chrono;

#ifndef NDEBUG

    void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam) {
        std::cout << "---------------" << std::endl;
        std::cout << "Debug message (" << id << "): " <<  message << std::endl;

        switch (source)
        {
            case GL_DEBUG_SOURCE_API:             std::cout << "Source: API\n"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System\n"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler\n"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party\n"; break;
            case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application\n"; break;
            case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other\n"; break;
        }

        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error\n"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour\n"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour\n"; break; 
            case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability\n"; break;
            case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance\n"; break;
            case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker\n"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group\n"; break;
            case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group\n"; break;
            case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other\n"; break;
        }

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high\n"; break;
            case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium\n"; break;
            case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low\n"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification\n"; break;
        }
        std::cout << std::endl;
    }
#endif


engine_runtime::engine_runtime(game_window& window) 
    : m_root_node(new scene_node("GLOBAL_ROOT", scene_node::node_type::ROOT)), m_window(window), m_global_clock(0.0f) {

    std::cerr << "Runtime created (" << &m_window << ")" << std::endl; 
    s_instance = this;

    #ifndef NDEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    #endif

    m_renderer.init();
    m_events.apply_callbacks(window);
    
    glfwSetWindowCloseCallback(window.props().glfw_handle, [](GLFWwindow*) { engine_runtime::s_instance->m_window.close(); });
    glViewport(0, 0, window.props().current_mode.size().x, window.props().current_mode.size().y);
};

engine_runtime::~engine_runtime() {

    /* Delete the scene */
    delete m_root_node;
}

void engine_runtime::start() {           

    /* Setup timekeeping */
    system_clock::time_point tp_prev = system_clock::now(), 
                             tp_now;

    float physics_delta = 0.0f;
    float physics_interval = project_settings::physics_interval();

    /* Load the initial scene */
    auto initial_scene = assets::loader::load<assets::scene_template>(project_settings::default_scene_path());
    root_node(initial_scene->instantiate());

    /* Check if renderer has a valid camera */
    if (!m_renderer.has_active_camera())
        cerr << "No main camera found in the scene! For rendering to work, you'll need to set one up manually" << std::endl;

    /* Mainloop */
    while (!m_window.props().is_closing) {
        
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
        m_renderer.draw_scene();

        /* Display new frame */
        glfwSwapBuffers(m_window.props().glfw_handle);
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
