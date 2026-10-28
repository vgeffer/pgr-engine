/// 
/// @file shader.hpp
/// @author geffevil
///
#pragma once

#include "Common/Common.hpp"
#include "asset.hpp"
#include "vulkan/vulkan.hpp"

namespace pgr::Core::Assets {
    class ShaderModule : public AssetBase {
        
        public:
        public:
            /// @brief Constructor for the shader_stage class
            /// Loads, processes and compiles the shader
            ///
            /// @param path Filesystem path of the shader
            ShaderModule(const char* modulePath);
            ~ShaderModule();
    
           
            PGR_NODISCARD inline operator vk::ShaderModule() const noexcept { return mModule; }

        private:
            vk::ShaderModule mModule;
    };
}
