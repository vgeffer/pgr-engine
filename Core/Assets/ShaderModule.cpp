#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "ShaderModule.hpp"
#include <App/Application.hpp>
#include "vulkan/vulkan.hpp"

using namespace pgr::Core::Assets;
using App = pgr::App::Application;

ShaderModule::ShaderModule(const char* modulePath) {     
    
    std::ifstream moduleSourceStream = std::ifstream(modulePath, std::ios::in | std::ios::binary);

    if (!moduleSourceStream.is_open())
        throw std::runtime_error("Unable to open shader file " + std::string(modulePath));
    
    std::vector<char> moduleSource = std::vector<char>(
        std::istreambuf_iterator<char>(moduleSourceStream), 
        std::istreambuf_iterator<char>()
    );

    if (!moduleSourceStream.eof() && moduleSourceStream.fail())        
		throw std::runtime_error("Unable to read from shader file " + std::string(modulePath));

    moduleSourceStream.close();

    std::cout << modulePath << ": " << moduleSource.size() << std::endl;
    

    vk::ShaderModuleCreateInfo moduleInfo = vk::ShaderModuleCreateInfo()
        .setCodeSize(moduleSource.size())
        .setPCode(reinterpret_cast<const uint32_t*>(moduleSource.data()));

    mModule = static_cast<vk::Device>(::App::UsedDevice()).createShaderModule(moduleInfo);

}

ShaderModule::~ShaderModule() {

    static_cast<vk::Device>(::App::UsedDevice()).destroyShaderModule(mModule);
}
