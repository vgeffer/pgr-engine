#find_package(Python3 REQUIRED COMPONENTS Interpreter)
#
#execute_process(
#    COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/utils/gen_backend.py 
#    RESULT_VARIABLE _PYTHON_RETURN_CODE
#    OUTPUT_VARIABLE
#)
#if (${_PYTHON_RETURN_CODE} EQUAL "0")
#    add_custom_command(
#        OUTPUT ${SHADERS_OUT}
#        COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/utils/gen_shaders.py ${SHADERS}
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#        COMMENT "Generating shader sources..."
#    )
#    add_custom_target(
#        gen_backend ALL
#        DEPENDS ${PROJECT_SOURCE_DIR}/utils/gen_shaders.py ${SHADERS}
#        ${SHADERS_OUT}
#    )
#else()
#    message(WARNING "Unable to get list of shaders to be generated! Build may fail...")
#endif()

add_subdirectory(desktop/vulkan)
add_subdirectory(webgl)
