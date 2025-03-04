#pragma once
#include <glm/gtc/quaternion.hpp> 
#include <glm/glm.hpp>
#include <vector>

namespace nodes {
    class scene_node {

        public:
            glm::vec3 position, scale;
            glm::quat rotation;
            virtual void update(float delta) {};

            void add_child(scene_node* node);

            scene_node* child(int index);
            std::vector<scene_node*>& children();
        
        
            glm::mat4x4& get_model_mat();

            bool enabled() const;
            bool enabled(bool _value);

        protected:
            explicit scene_node();

        private:
            std::vector<scene_node*> _children;
            bool _enabled;
    };
}