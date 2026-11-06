cmake_minimum_required(VERSION 3.30.0)
project(PgrEngine C CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

option(LOCAL_DEPENDENCIES, false)

# Remote Dependencies
FetchContent_Declare(
    vmahpp
    URL      https://github.com/YaaZ/VulkanMemoryAllocator-Hpp/releases/download/v3.5.0+1/VulkanMemoryAllocator-Hpp-3.5.0.tar.gz
    URL_HASH SHA256=b8d8d00e9575a6ce8c217bb6857bacf391212cae21f2c22695beb9609f2ef4a0 # Copy from the asset list on the release page
)
FetchContent_Declare (
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
)

if (${LOCAL_DEPENDENCIES})
    find_package(glfw3 3.4 REQUIRED)
    find_package(VulkanMemoryAllocator-Hpp CONFIG REQUIRED)
else()
    message("Downloading dependencies")
    FetchContent_MakeAvailable(glfw)
    FetchContent_MakeAvailable(vmahpp)
endif()

# Always local resurces
find_package(Python3 3.9 REQUIRED COMPONENTS Interpreter)
find_package(Vulkan 1.4 REQUIRED COMPONENTS glslc)

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
    "Core/Rendering/VertexFormat.cpp"
    "Core/Rendering/VertexFormat.hpp"
    "Core/Memory/GpuMemory.hpp"
    "Core/Memory/GpuMemory.cpp"
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
target_link_libraries(engine glfw Vulkan::Vulkan VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp)
target_compile_definitions(engine PRIVATE PGR_EXPORT)





