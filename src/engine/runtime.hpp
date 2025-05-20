#pragma once

#include "rendering/renderer.hpp"
#include "events.hpp"
#include "scene/scene_node.hpp"
#include "game_window.hpp"

#include <glm/fwd.hpp>

/// @brief Main runtime class of the engine
class engine_runtime {
   
    public:
        /// @brief Creates and initializes the runtime
        ///
        /// @param window Window to tie the runtime to
        engine_runtime(game_window& window);
        ~engine_runtime();

        /// @brief Starts the mainloop
        void start(); 

        inline static engine_runtime* instance() { return s_instance; }
        
        inline const scene::scene_node* root_node() const { return m_root_node; }

        /// @brief Sets the scene's root node
        /// @param node New root node
        /// @returns New root node
        scene::scene_node* root_node(scene::scene_node* node);
        
        inline float global_clock() const { return m_global_clock; }
        inline game_window& window() const { return m_window; }

    private:
        inline static engine_runtime* s_instance = nullptr;
        scene::scene_node* m_root_node; ///< Root node of the scene
        game_window& m_window;          ///< Window to render to
        float m_global_clock;           ///< Global clock

        events m_events;                ///< Event handler instance
        rendering::renderer m_renderer; ///< Renderer instance
};
