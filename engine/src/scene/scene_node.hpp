#pragma once
#include <functional>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include "../utils/type_list.hpp"
#include "../utils/observer_ptr.hpp"
#include "../utils/resource.hpp"

#define REGISTER_COMPONENT(T)                                                    \
    struct scene::_internal::component_registry::registration _spawn_reg_##T(#T, \
        [] (scene::scene_node* n, const utils::resource& res) {                  \
            static_assert(std::is_base_of<scene::node_component, T>::value, ""); \
            n->create_component<T>(n, res);                                      \
        }                                                                        \
    );  

namespace scene {
    class scene_node {

        public:
            enum class node_type {
                GENERIC,    ///< Generic node 
                ROOT        ///< Root node. There may be only one root node in the scene at any time
            };

            /// @brief Constructs generic scene node named @c name
            /// @param name Name of the node
            scene_node(const std::string& name);

            /// @brief Constructs node of type @c type with name @c name
            /// @param name Name of the node
            /// @param type Type of the node
            scene_node(const std::string& name, node_type type);

            /// @brief Constructs generic node from JSON
            /// @param res JSON resource
            scene_node(const utils::resource& res);

            scene_node(const scene_node&) = delete;
            virtual ~scene_node();

            /// @brief Calls the update of the node
            /// @param delta Delta time 
            void update_node(float delta);
            void prepare_draw(const glm::mat4x4& parent_transform);

            /// @brief Adds child node
            void add_child(scene_node* node);
            
            scene_node* child(const std::string& name) const;
            glm::mat4x4 model_mat() const;

            inline const std::unordered_map<std::string, scene_node*>& children() const { return m_children; }
            inline node_type type() const { return m_type; }

            inline bool enabled() const { return m_enabled; }
            inline bool enabled(bool value) { return m_enabled = value; }

            inline bool visible() const { return m_visible; }
            bool visible(bool value);

            template<class T>
                inline utils::observer_ptr<T> component() { return utils::observer_cast<T>(m_components.get<T>().observer()); } 
            template<class T>
                inline bool has_component() { return m_components.has<T>(); }
            template<class T, typename ...Args>
                inline void create_component(Args... args) {
                    return m_components.set<T>(utils::observer_cast<class node_component>(utils::make_observable<T>(args...))); 
                }

            bool in_active_scene() const { return m_in_scene; };
        
        public:
            glm::vec3 position, scale;
            glm::quat rotation;

        private:
            void m_on_scene_enter();
            bool m_check_cycles();

        private:
            std::string m_name;
            std::unordered_map<std::string, scene_node*> m_children;
            utils::type_list<utils::observable_ptr<class node_component>> m_components;

            scene_node* m_parent;
            bool m_enabled,
                 m_visible,
                 m_in_scene;

            node_type m_type;
    };

    /// @brief Base class for the node components
    ///
    /// This class has no functionality by itself, it only provides a common scene interface for derived classes
    class node_component {

        public:
            virtual ~node_component() = default;

            /// @brief Callback, called when parent node enters the scene for the first time
            virtual void scene_enter() {}

            /// @brief Callback, called when the parent node exits (is deleted from) the scene
            virtual void scene_exit() {}

            /// @brief Callback, prepares underlying object for rendering
            /// @param parent_transform Model matrix of the parent node
            virtual void prepare_draw(const glm::mat4x4& parent_transform) {}

            /// @brief Callback, visibility notifier
            virtual void visibility_changed() {}

            /// @brief Callback, called once per frame
            /// @param delta Time elapsed since the last frame (in seconds)
            virtual void update(float delta) {}

            /// @brief Callback, called in constant, periodic intervals
            /// @param delta Time elapsed since last call
            virtual void fixed_update(float delta) {}

            inline scene_node* parent() const { return m_parent; }

        protected:
            node_component(scene_node* parent) : m_parent(parent) {}

        private:
            scene_node* m_parent;   ///< Parent node
    };

    namespace _internal {

        /// @brief Class providing node component auto-registration funcitonality
        /// @private
        class component_registry {
              
            public:
                using spawner_func = std::function<void(scene_node*, const utils::resource&)>;
                inline static std::unordered_map<std::string, spawner_func> registered_components = std::unordered_map<std::string, spawner_func>();

                struct registration {
                    registration(const std::string_view key, spawner_func spawner) {
                        registered_components.emplace(key, spawner);
                    }
                };    
        };
    }
}

MAKE_JSON_PARSABLE(scene::scene_node*, [] (const utils::resource& res) {
    return new scene::scene_node(res);
});
