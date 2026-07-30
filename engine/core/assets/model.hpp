///
/// @file model.hpp
/// @author geffevil
///
#pragma once 
#include "../rendering/mesh.hpp"
#include <string>

namespace assets {
    /// @brief Mesh provider asset, wrapper around a model file
    /// @see rendering::mesh
    class model : public asset, public rendering::mesh {
       
        public:
            /// @brief Asset constructor, loads and sets up model
            ///
            /// Loads the asset file and does the neceseary pre-processing (computes normals, etc)
            /// @warning This function loads only @b FIRST mesh present in the model file, any others are disregarded
            ///
            /// @param path Filesystem path to the desired asset
            model(const std::string& path);

            /// @brief Default destructor, just to ensure everything is destroyed properly 
            ///
            /// This destructor is explicitly define just to @b reaaaly @b ensure that the destructor
            /// of the underlying @c rendering::mesh gets called
            ~model() override = default;
    };
}
