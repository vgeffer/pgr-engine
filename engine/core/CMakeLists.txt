find_package(assimp REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_library(pgrengine SHARED
    "app/application.hpp"
    "app/app_config.hpp"
    "app/application.cpp"
    "app/app_config.cpp"
    "utils/type_list.hpp"
    "utils/observer_ptr.hpp"
    "utils/algorithms.hpp"
    "utils/group_list.hpp"
    "window/video_mode.cpp"
    "window/events.hpp"
    "window/video_mode.hpp"
    "window/events.cpp"
    "window/key_code.hpp"
    "rendering/mesh.cpp"
    "rendering/material.hpp"
    "rendering/renderer.hpp"
    "rendering/camera.cpp"
    "rendering/light.cpp"
    "rendering/light.hpp"
    "rendering/mesh.hpp"
    "rendering/material.cpp"
    "rendering/renderer.cpp"
    "rendering/camera.hpp"
    "physics/raycast.cc"
    "physics/bounding_volumes.hpp"
    "physics/raycast.hpp"
    "physics/physics.hpp"
    "physics/bounding_volumes.cc"
    "scene/scene_node.hpp"
    "scene/scene_node.cpp"
    "assets/loader.hpp"
    "assets/displacement.cpp"
    "assets/scene.cpp"
    "assets/cubemap.cpp"
    "assets/texture.cpp"
    "assets/model.hpp"
    "assets/shader.cpp"
    "assets/cubemap.hpp"
    "assets/texture.hpp"
    "assets/model.cpp"
    "assets/shader.hpp"
    "assets/asset.hpp"
    "assets/displacement.hpp"
    "assets/scene.hpp"
    "rendering/meshes/skybox.hpp"
    "rendering/meshes/quad.hpp"
    "rendering/meshes/billboard.hpp"
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
