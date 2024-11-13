macro(add_sample)

    get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp *.h *.hpp)

    add_executable(${PROJECT_NAME})
    target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE prism)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}")
    set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

    file(GLOB_RECURSE GLSL_SHADERS *.glsl)
    unset(SPV_SHADERS)
    foreach(GLSL_SHADER ${GLSL_SHADERS})
        get_filename_component(SHADER_NAME ${GLSL_SHADER} NAME_WLE)
        set(SPV_SHADER ${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.spv)
        compile_glsl(${GLSL_SHADER} ${SPV_SHADER} SPV_SHADERS)
    endforeach()
    add_custom_target(${PROJECT_NAME}_shaders DEPENDS ${SPV_SHADERS})

    add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)
    
endmacro(add_sample)

include(glsl)

add_subdirectory(00_hello_world)
add_subdirectory(01_offscreen)
add_subdirectory(02_compute_pipeline)
add_subdirectory(03_graphics_pipeline)