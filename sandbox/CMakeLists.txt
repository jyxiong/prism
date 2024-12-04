macro(compile_glsl GLSL_SHADER SPV_SHADER SPV_SHADERS)
    add_custom_command(
        OUTPUT ${SPV_SHADER}
        COMMAND glslang-standalone
        ARGS ${GLSL_SHADER} --target-env vulkan1.3 -o ${SPV_SHADER} 
        MAIN_DEPENDENCY ${GLSL_SHADER}
        WORKING_DIRECTORY ${CMAKE_SHADERS_OUTPUT_DIRECTORY}
        COMMENT "Compiling shader: ${GLSL_SHADER}"
    )
    list(APPEND ${SPV_SHADERS} ${SPV_SHADER})
endmacro()

macro(copy_texture SRC_TEXTURE DST_TEXTURE DST_TEXTURES)
    add_custom_command(
        OUTPUT ${DST_TEXTURE}
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${SRC_TEXTURE} ${DST_TEXTURE}
        MAIN_DEPENDENCY ${SRC_TEXTURE}
        COMMENT "Copying texture: ${SRC_TEXTURE}"
    )
    list(APPEND ${DST_TEXTURES} ${DST_TEXTURE})
endmacro()

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

    file(GLOB_RECURSE TEXTURES *.png *.jpg *.jpeg *.bmp)
    unset(DST_TEXTURES)
    foreach(TEXTURE ${TEXTURES})
        get_filename_component(TEXTURE_NAME ${TEXTURE} NAME)
        set(DST_TEXTURE ${CMAKE_CURRENT_BINARY_DIR}/textures/${TEXTURE_NAME})
        copy_texture(${TEXTURE} ${DST_TEXTURE} DST_TEXTURES)
    endforeach()
    add_custom_target(${PROJECT_NAME}_textures DEPENDS ${DST_TEXTURES})

    add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)
    add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_textures)
    
endmacro(add_sample)

add_subdirectory(00_hello_world)
add_subdirectory(01_offscreen)
add_subdirectory(02_compute_pipeline)
add_subdirectory(03_graphics_pipeline)
add_subdirectory(04_render_frame)
add_subdirectory(05_render_context)
add_subdirectory(06_shader_input)
add_subdirectory(07_texture)