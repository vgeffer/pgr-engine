find_package(assimp REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_library(pgrengine SHARED
"app/application.cpp"
"app/app_config.cpp"
"runtime/backend.cpp"
"utils/resource.cpp"
"utils/gpu_memory.cpp"
"utils/debug.cpp"
"utils/project_settings.cpp"
"window/video_mode.cpp"
"rendering/mesh.cpp"
"rendering/camera.cpp"
"rendering/light.cpp"
"rendering/material.cpp"
"rendering/renderer.cpp"
"engine.cpp"
"scene/scene_node.cpp"
"assets/displacement.cpp"
"assets/scene.cpp"
"assets/cubemap.cpp"
"assets/texture.cpp"
"assets/shader.cpp"
"assets/model.cpp"
"engine.hpp"
"app/application.hpp"
"app/app_config.hpp"
"runtime/refcounter.hpp"
"runtime/backend.hpp"
"utils/project_settings.hpp"
"utils/debug.hpp"
"utils/type_list.hpp"
"utils/gpu_memory.hpp"
"utils/buffer.hpp"
"utils/observer_ptr.hpp"
"utils/algorithms.hpp"
"utils/group_list.hpp"
"utils/resource.hpp"
"common/capabilities.hpp"
"common/object.hpp"
"window/video_mode.hpp"
"window/key_code.hpp"
"rendering/material.hpp"
"rendering/renderer.hpp"
"rendering/meshes/skybox.hpp"
"rendering/meshes/quad.hpp"
"rendering/meshes/billboard.hpp"
"rendering/light.hpp"
"rendering/shaders/quad.gen.hpp"
"rendering/shaders/combination.gen.hpp"
"rendering/shaders/skybox_frag.gen.hpp"
"rendering/shaders/skybox_vert.gen.hpp"
"rendering/mesh.hpp"
"rendering/camera.hpp"
"physics/bounding_volumes.hpp"
"physics/raycast.hpp"
"physics/physics.hpp"
"scene/scene_node.hpp"
"events/event_provider.hpp"
"assets/loader.hpp"
"assets/model.hpp"
"assets/cubemap.hpp"
"assets/texture.hpp"
"assets/shader.hpp"
"assets/asset.hpp"
"assets/displacement.hpp"
"assets/scene.hpp"
)

set(SHADERS
    rendering/shaders/combination.glsl
    rendering/shaders/quad.glsl
    rendering/shaders/skybox_frag.glsl
    rendering/shaders/skybox_vert.glsl
)

execute_process(
    COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/utils/gen_shaders.py --outfiles ${SHADERS}
    RESULT_VARIABLE _PYTHON_RETURN_CODE
    OUTPUT_VARIABLE SHADERS_OUT
)
if (${_PYTHON_RETURN_CODE} EQUAL "0")
    add_custom_command(
        OUTPUT ${SHADERS_OUT}
        COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/utils/gen_shaders.py ${SHADERS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating shader sources..."
    )
    add_custom_target(
        gen_shaders ALL
        DEPENDS ${PROJECT_SOURCE_DIR}/utils/gen_shaders.py ${SHADERS}
        ${SHADERS_OUT}
    )
    add_dependencies(pgrengine gen_shaders)
else()
    message(WARNING "Unable to get list of shaders to be generated! Build may fail...")
endif()

include_directories(pgrengine ${PROJECT_SOURCE_DIR}/engine)
target_link_libraries(pgrengine assimp::assimp)
target_compile_definitions(pgrengine PRIVATE PGR_EXPORT)

# find ../engine/engine/core -name '*.cpp' | cut -c 23- | sed -e 's/.*/"&"/' >> ../engine/engine/core/CMakeLists.txt && find ../engine/engine/core -name '*.hpp' | cut -c 23- | sed -e 's/.*/"&"/' >> ../engine/engine/core/CMakeLists.txt


