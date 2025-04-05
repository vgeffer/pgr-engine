#pragma once
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/glm.hpp>
#include <vector>
#include "../utils/type_list.hpp"
#include "../utils/observer_ptr.hpp"

namespace nodes {

    class node_component {
        
        public:
            virtual ~node_component() = default;

            virtual void on_scene_enter() {}
            virtual void update(float delta) {}
            virtual void physics_update(float delta) {}

        protected:
            node_component() = default;
    };

    class scene_node {

        public:
            enum class node_type {
                ROOT, CAMERA, GENERIC
            };

            scene_node();
            scene_node(node_type type);

            scene_node(const scene_node&) = delete;
            virtual ~scene_node() = default;

            void node_update(float delta);
            void add_child(scene_node* node);

            scene_node* child(size_t index) const;
            inline glm::mat4x4 model_mat() const { return _model_mat; }

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
                    return _components.set<T>(utils::observer_cast<node_component>(utils::make_observable<T>(args...))); 
                }

            bool in_active_scene() const { return _in_scene; };

            inline glm::vec3 position() const { return _position; }
            glm::vec3 position(const glm::vec3& position);
            
            inline glm::quat rotation() const { return _rotation; }
            glm::quat rotation(const glm::quat& rotation);

            inline glm::vec3 scale() const { return _scale; }
            glm::vec3 scale(const glm::vec3& scale); 

        protected:
            /* Update functions */
            virtual void update (float delta) {}
            virtual void physics_update(float interval) {}

        private:
            std::vector<scene_node*> _children;
            utils::type_list<utils::observable_ptr<node_component>> _components;

            scene_node* _parent;
            bool _enabled;
            bool _in_scene;

            node_type _type;
            glm::vec3 _position, _scale;
            glm::quat _rotation;
            glm::mat4x4 _model_mat;

            void _on_scene_enter();
            bool _check_cycles();
    };
}