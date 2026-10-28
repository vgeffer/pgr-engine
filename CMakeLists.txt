cmake_minimum_required(VERSION 3.30.0)
project(PgrEngine C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(glfw3 3.5 REQUIRED)
find_package(Vulkan 1.1 REQUIRED COMPONENTS glslc)


# Project: Engine lib 
add_library(engine SHARED
    "App/Application.hpp"
    "App/AppManifest.cpp"
    "App/Application.cpp"
    "App/AppManifest.hpp"
    "Common/SystemMacros.hpp"
    "Common/Exceptions.hpp"
    "Common/Common.hpp"
    "Core/Utils/resource.cpp"
    "Core/Utils/type_list.hpp"
    "Core/Utils/resource.hpp"
    "Core/Rendering/Renderer.hpp"
    "Core/Rendering/Device.cpp"
    "Core/Rendering/Device.hpp"
    "Core/Rendering/Renderer.cpp"
    "Core/Assets/ShaderModule.cpp"
    "Core/Assets/ShaderModule.hpp"
    "Core/Assets/Asset.hpp"
    "Core/Presentation/Window.cpp"
    "Core/Presentation/SwapChain.hpp"
    "Core/Presentation/Window.hpp"
    "Core/Presentation/SwapChain.cpp"
    "Core/Presentation/KeyCode.hpp"
    "Core/Debug/VulkanValidation.cpp"
    "Core/Debug/VulkanValidation.hpp"
)

add_library(PgrEngine::Engine ALIAS engine)

target_include_directories(engine PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(engine glfw Vulkan::Vulkan Vulkan::shaderc_combined)
target_compile_definitions(engine PRIVATE PGR_EXPORT)





