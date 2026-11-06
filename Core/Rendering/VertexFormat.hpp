#pragma once


#include "vulkan/vulkan.hpp"
#include <string>
#include <Common/Common.hpp>
#include <tuple>
#include <vector>

namespace pgr::Core::Rendering {

    class VertexFormat {

        public:
            enum class InputSize : int { eFloat, eVec2f, eVec3f, eVec4f };
            enum class InputRate : int { ePerVertex = (int)vk::VertexInputRate::eVertex, PerInstance = (int)vk::VertexInputRate::eInstance };

        public:
            PGR_NODISCARD inline VertexFormat& AddBuffer(InputRate input) { mBuffers.emplace_back(input); return *this; }
            PGR_NODISCARD inline VertexFormat& AddInput(InputSize size, const std::string& name) { mInputs.emplace_back(name, size, mBuffers.size() - 1); return *this; }
            
            PGR_NODISCARD std::string MakeShaderHeader();
            PGR_NODISCARD std::vector<vk::VertexInputBindingDescription> MakeBindingDescriptions();
            PGR_NODISCARD std::vector<vk::VertexInputAttributeDescription> MakeVertexAttributes();

        private:
            std::vector<InputRate> mBuffers;
            std::vector<std::tuple<std::string, InputSize, uint32_t>> mInputs;
    };
}

