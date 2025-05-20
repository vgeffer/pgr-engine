///
/// @file asset.hpp
/// @author geffevil
///
#pragma once

namespace assets {

    /// @brief Base class for all pgr-engine's assets
    ///
    /// To qualify as asset, class must publicly derive from this class
    /// Class must also provide constructor which takes one parameter, @c{const string& path}
    /// The above conditions must be met to be able to load the asset using @c assets::loader::load
    class asset {
        public:
            /// @brief Default destructor for class - assuring proper construction/destruction 
            virtual ~asset() = default;
    };
}