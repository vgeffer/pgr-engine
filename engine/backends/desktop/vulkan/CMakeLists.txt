find_package(Vulkan REQUIRED COMPONENTS glslc)
find_package(glfw3 3.4 REQUIRED)


set(SOURCES
    "backend.cpp"
    "device.cpp"
    "presentation.cpp"
    "device_features.cpp"
    "vulkan_backend.cpp"
    "device_features.hpp"
    "vulkan_backend.hpp"
    "presentation.hpp"
    "device.hpp"
)

add_library(pgr_vulkan_backend SHARED 
    "backend.cpp"
    "device_features.hpp"
    "vulkan_backend.hpp"
    "structs.hpp"
    "window.cpp"
    "presentation.hpp"
    "device.cpp"
    "gpu_memory.cpp"
    "graphics.cpp"
    "presentation.cpp"
    "device.hpp"
    "gpu_memory.hpp"
    "device_features.cpp"
    "vulkan_backend.cpp"
)
target_link_libraries(pgr_vulkan_backend Vulkan::Vulkan glfw)
target_compile_definitions(pgr_vulkan_backend PRIVATE PGR_EXPORT)
include_directories(pgr_vulkan_backend ${PROJECT_SOURCE_DIR}/engine)
