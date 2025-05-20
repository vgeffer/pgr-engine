#pragma once
#include "../scene/scene_node.hpp"
#include "asset.hpp"
#include <string>


namespace assets {

    /// @brief Scene template asset
    ///
    /// Loads template of the scene, allowing it to be instantiated
    class scene_template : public asset {
    
        public:
            /// @brief Asset constructor
            ///
            /// Loads scene resource, preparig it for instatiation
            ///
            /// @param path Filesystem path to the desired asset
            /// @see assets::asset
            scene_template(const std::string path);

            /// @brief Defautl dectructor
            ~scene_template() override = default;

            /// @brief Instantiates scene from the template
            ///
            /// The scene is only instantiated and initalized, however it does not
            /// make it active. For that, you need to call @c runtime::root_node
            /// 
            /// @returns Pointer to the root node of the instantiated scene
            /// @see runtime::root_node
            scene::scene_node* instantiate();

        private:
            utils::resource m_scene_res;    ///< Resource containing the scene template
    };
};