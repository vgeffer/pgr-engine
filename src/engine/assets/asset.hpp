///
/// @file asset.hpp
/// @author geffevil
/// @brief A base class for all assets
///
#pragma once


namespace assets {

    class asset {
        public:
            /// @brief Default destructor for class - assuring proper construction/destruction 
            virtual ~asset() = default;
    };
}