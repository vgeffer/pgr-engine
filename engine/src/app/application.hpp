#pragma once

#include "../rendering/renderer.hpp"
#include "../window/events.hpp"
#include "../scene/scene_node.hpp"
#include "../utils/vulkan/instance.hpp"
#include "../assets/loader.hpp"
#include "app_config.hpp"

#include <glm/fwd.hpp>
#include <vulkan/vulkan.hpp>

/// @brief Main runtime class of the engine
namespace pgreng::app {
    class application {

        public:
            /// @brief Creates and initializes the runtime
            ///
            /// @param window Window to tie the runtime to
            application();
            ~application();

            /// @brief Starts the mainloop
            int start(); 

            inline const scene::scene_node* root_node() const { return m_root_node; }

            /// @brief Sets the scene's root node
            /// @param node New root node
            /// @returns New root node
            scene::scene_node* root_node(scene::scene_node* node);


            inline static float global_clock() noexcept { return s_instance->m_global_clock; }
            inline static const application& current() noexcept { return *s_instance; }
            inline static const utils::vulkan::instance& instance() noexcept { return *s_instance; }

            
        private:
            inline static application* s_instance = nullptr;
            scene::scene_node* m_root_node; ///< Root node of the scene
            float m_global_clock;           ///< Global clock

            app_config              m_config;

            window::events          m_events;   ///< Event handler instance
            rendering::renderer     m_renderer; ///< Renderer instance

            assets::loader          m_asset_loader;

            utils::vulkan::instance m_vk_instance;
            
    };
}