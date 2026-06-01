cmake_minimum_required(VERSION 3.30.0)
project(pgr_engine C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# Setup definitions
option(DISABLE_EDITOR "Disable editor compilation" NO)
option(GEN_DEV_ENV "Generate files for a dev environment" NO)

set(HAS_EDITOR NO)

# Helper functions
function (set_bool_not var_name var_value)
    if(${var_value})
        set(${var_name} NO PARENT_SCOPE)
    else()
        set(${var_name} YES PARENT_SCOPE)
    endif()
endfunction()

# Check if subprojects (editor, ...) exist
get_filename_component(editor_fs_path "${PROJECT_SOURCE_DIR}/editor" REALPATH)
if (EXISTS "${editor_fs_path}" AND EXISTS "${editor_fs_path}/CMakeLists.txt")
    if (DISABLE_EDITOR)
        set_bool_not(HAS_EDITOR ${DISABLE_EDITOR})
    else()
        set(HAS_EDITOR YES)
    endif()
endif()
message(STATUS "Build Editor? ${HAS_EDITOR}")

# Check if dev environment is supossed to be built
if (${GEN_DEV_ENV})
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    #TODO: Also build shaders
endif()
message(STATUS "Generating Environment? ${GEN_DEV_ENV}")

add_subdirectory(engine)
if (HAS_EDITOR AND ${HAS_EDITOR})
    add_subdirectory(editor)
endif()
