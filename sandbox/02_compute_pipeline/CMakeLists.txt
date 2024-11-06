get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp *.h *.hpp)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE prism)
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}")

include(glsl)

file(GLOB GLSL_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.comp)
unset(SPV_SHADERS)
foreach(GLSL_SHADER ${GLSL_SHADERS})
    get_filename_component(SHADER_NAME ${GLSL_SHADER} NAME_WE)
    set(SPV_SHADER ${CMAKE_SHADERS_OUTPUT_DIRECTORY}/${SHADER_NAME}.spv)
    compile_glsl(${GLSL_SHADER} ${SPV_SHADER} SPV_SHADERS)
endforeach()
add_custom_target(${PROJECT_NAME}_shaders DEPENDS ${SPV_SHADERS})

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)
