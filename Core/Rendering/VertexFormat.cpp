#include "VertexFormat.hpp"
#include "vulkan/vulkan.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>

using namespace pgr::Core::Rendering;

struct LUTEntry {
    vk::Format attrFormat;
    const char* glslType;
    uint32_t locationsRequired;
    size_t elementSize;
};

static constexpr LUTEntry LUT[] {
    /* InputSize::eFloat */LUTEntry{vk::Format::eR32Sfloat, "float", 1, sizeof(float) },
    /* InputSize::eVec2f */LUTEntry{vk::Format::eR32G32Sfloat, "vec2", 1, sizeof(glm::vec2) },
    /* InputSize::eVec3f */LUTEntry{vk::Format::eR32G32B32Sfloat, "vec3", 1, sizeof(glm::vec3) },
    /* InputSize::eVec4f */LUTEntry{vk::Format::eR32G32B32A32Sfloat, "vec4", 1, sizeof(glm::vec4) },
};

std::string VertexFormat::MakeShaderHeader() {

    std::stringstream headerLines;
    uint32_t slot = 0;

    for (const auto& input : mInputs) {

        assert((int)std::get<1>(input) < sizeof(LUT) / sizeof(LUT[0]));

        headerLines << "layout(location = " << slot << ") in "
                    << LUT[(int)std::get<1>(input)].glslType
                    << " " << std::get<0>(input) << ";\n";
        slot += LUT[(int)std::get<1>(input)].locationsRequired;
    }
    return headerLines.str();
}

std::vector<vk::VertexInputBindingDescription> VertexFormat::MakeBindingDescriptions() {

    std::vector<vk::VertexInputBindingDescription> output;

    uint32_t lastBuffer = 0xFFFF; //Doesn't matter, just not 0
    size_t size = 0;

    for (const auto& input : mInputs) {

        assert((int)std::get<1>(input) < sizeof(LUT) / sizeof(LUT[0]));
        assert(std::get<2>(input) < mBuffers.size());

        if (std::get<2>(input) != lastBuffer) {

            /* Output new description - if there is anything to output */
            if (size > 0) {
                vk::VertexInputBindingDescription binding = vk::VertexInputBindingDescription()
                    .setBinding(lastBuffer)
                    .setInputRate(static_cast<vk::VertexInputRate>(mBuffers[lastBuffer]))
                    .setStride(size);   
                output.push_back(binding);


                std::cout << "binding: " << lastBuffer << "\n" 
                          << "inputRate: " << (int)mBuffers[lastBuffer] << "\n"
                          << "stride: " << size << std::endl;
            }

            size = 0;
            lastBuffer = std::get<2>(input);
        }

        size += LUT[(int)std::get<1>(input)].elementSize;
    }

    /* output last one - if. there is anything left */
    if (size > 0) {
        vk::VertexInputBindingDescription binding = vk::VertexInputBindingDescription()
            .setBinding(lastBuffer)
            .setInputRate(static_cast<vk::VertexInputRate>(mBuffers[lastBuffer]))
            .setStride(size);   
        output.push_back(binding);

        std::cout << "Binding: " << lastBuffer << "\n" 
                  << "InputRate: " << (int)mBuffers[lastBuffer] << "\n"
                  << "Stride: " << size << std::endl;
    }

    return output;
}

std::vector<vk::VertexInputAttributeDescription> VertexFormat::MakeVertexAttributes() {
    
    std::vector<vk::VertexInputAttributeDescription> output;
    
    uint32_t lastBuffer = 0xFFFF; //Doesn't matter, just not 0
    uint32_t location = 0;
    size_t offset = 0;

    for (const auto& input : mInputs)  {

        assert((int)std::get<1>(input) < sizeof(LUT) / sizeof(LUT[0]));
        assert(std::get<2>(input) < mBuffers.size());

        if (std::get<2>(input) != lastBuffer) {

            offset = 0; //start offset from 0 if switching buffers
            lastBuffer = std::get<2>(input);
        }
        
        vk::VertexInputAttributeDescription desc = vk::VertexInputAttributeDescription()
            .setBinding(std::get<2>(input))
            .setLocation(location)
            .setFormat(LUT[(int)std::get<1>(input)].attrFormat)
            .setOffset(offset); //TODO: Alignment

        location += LUT[(int)std::get<1>(input)].locationsRequired;
        offset   += LUT[(int)std::get<1>(input)].elementSize; //For now assume packed structure
        output.push_back(desc);
    }

    return output;
}
