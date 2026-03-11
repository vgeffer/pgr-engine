///
/// @file camera.hpp
/// @author geffevil
///
#pragma once
#include <glm/glm.hpp>
#include "../../lib/glad/glad.h"
#include "../scene/scene_node.hpp"


namespace rendering {
    /// @brief Camera class, handling view and projection 
    class camera : public scene::node_component {
    
        public:
            /// @brief Scene component constructor
            ///
            /// Constructs camera as described in scene file
            ///
            /// @param parent Parent node of this component
            /// @param res Component's resource
            /// @see scene::node_component
            camera(scene::scene_node* parent, const utils::resource& res);

            /// @brief Camera constructor
            ///
            /// Constructs new camera from given params
            ///
            /// @param parent Parent node of this component
            /// @param fov Camera's Field of View
            /// @param near Camera's near clipping plane
            /// @param far Camera's far clipping plane
            /// @param main Wether or not to activate the camera when it enters the scene
            /// @see scene::node_component
            camera(scene::scene_node* parent, float fov, float near, float far, bool main);
            ~camera() override;

            glm::mat4x4 view() const;
            glm::mat4x4 projection() const;

            const glm::vec3 up() const;
            const glm::vec3 forward() const;

            /* Setting projection parameters also updates stored matrices */
            inline float fov() const { return m_fov; }
            float fov(const float& fov);
            
            inline float near() const { return m_near; }
            float near(const float& near);
            
            inline float far() const { return m_far; }
            float far(const float& far);

            /// @brief Getter for the camera data
            /// @returns OpenGL handle for the buffer containing camera data
            inline GLuint camera_data() const { return m_camera_data; }

            /// @brief Makes camera active
            ///
            /// Sets this camera up as an active camera and begins rendering through it
            void make_active();

        private:
            void scene_enter() override;
            void prepare_draw(const glm::mat4x4& parent_matrix) override;

            float m_fov, m_near, m_far;
            bool m_main;
            GLuint m_camera_data;
    };
}