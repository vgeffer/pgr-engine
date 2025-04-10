#pragma once
#include <functional>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include "../utils/type_list.hpp"
#include "../utils/observer_ptr.hpp"
#include "../utils/resource.hpp"

#define REGISTER_COMPONENT(T)                                                    \
    struct scene::_internal::component_registry::registration _spawn_reg_##T(#T, \
        [] (scene::scene_node& n, const utils::resource& res) {                  \
            n.create_component<T>(res);                                          \
        }                                                                        \
    );  

namespace scene {
    class scene_node {

        public:
            enum class node_type {
                ROOT, GENERIC
            };

            scene_node();
            scene_node(node_type type);
            scene_node(const utils::resource& res);

            scene_node(const scene_node&) = delete;
            virtual ~scene_node();

            void node_update(float delta);
            void add_child(scene_node* node);

            scene_node* child(size_t index) const;
            glm::mat4x4 model_mat() const;

            inline std::vector<scene_node*>& children() { return _children; }
            inline node_type type() const { return _type; }

            inline bool enabled() const { return _enabled; }
            inline bool enabled(bool _value) { return _enabled = _value; }

            template<class T>
                inline utils::observer_ptr<T> component() { return utils::observer_cast<T>(_components.get<T>().observer()); } 
            template<class T>
                inline bool has_component() { return _components.has<T>(); }
            template<class T, typename ...Args>
                inline void create_component(Args... args) {
                    return _components.set<T>(utils::observer_cast<class node_component>(utils::make_observable<T>(args...))); 
                }

            bool in_active_scene() const { return _in_scene; };

            inline glm::vec3 position() const { return _position; }
            glm::vec3 position(const glm::vec3& position) { return _position = position; }
            
            inline glm::quat rotation() const { return _rotation; }
            glm::quat rotation(const glm::quat& rotation) { return _rotation = rotation; }

            inline glm::vec3 scale() const { return _scale; }
            glm::vec3 scale(const glm::vec3& scale) { return _scale = scale; }

        protected:
            /* Update functions */
            virtual void update (float delta) {}
            virtual void physics_update(float interval) {}

        private:
            std::vector<scene_node*> _children;
            utils::type_list<utils::observable_ptr<class node_component>> _components;

            scene_node* _parent;
            bool _enabled;
            bool _in_scene;

            node_type _type;
            glm::vec3 _position, _scale;
            glm::quat _rotation;

            void _on_scene_enter();
            bool _check_cycles();
    };

    class node_component {

        public:
            virtual ~node_component() = default;

            virtual void scene_enter() {}
            virtual void scene_exit() {}
            virtual void update(float delta) {}
            virtual void physics_update(float delta) {}

        protected:
            node_component() = default;
            scene_node* _parent;
    };

    namespace _internal {

        class component_registry {
              
            public:
                using spawner_func = std::function<void(scene_node&, const utils::resource&)>;
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
