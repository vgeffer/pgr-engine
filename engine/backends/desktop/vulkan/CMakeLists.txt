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

)
add_library(pgr_vulkan_backend_static STATIC 

)

target_link_libraries(pgr_vulkan_backend Vulkan::Vulkan glfw3)
target_compile_definitions(pgr_vulkan_backend PRIVATE PGR_EXPORT)
include_directories(pgr_vulkan_backend_static ${PROJECT_SOURCE_DIR}/engine)

target_link_libraries(pgr_vulkan_backend Vulkan::Vulkan glfw3)
target_compile_definitions(pgr_vulkan_backend_static PRIVATE PGR_EXPORT STATIC)
include_directories(pgr_vulkan_backend_static ${PROJECT_SOURCE_DIR}/engine)

# find ../engine/engine/backends/desktop/vulkan -name '*.cpp' | cut -c 42- | sed -e 's/.*/"&"/' >> ../engine/engine/backends/desktop/vulkan/CMakeLists.txt && find ../engine/engine/backends/desktop/vulkan -name '*.hpp' | cut -c 42- | sed -e 's/.*/"&"/' >> ../engine/engine/backends/desktop/vulkan/CMakeLists.txt
