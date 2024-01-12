project(vulkan_tutorial)

# 添加glslangValidator
add_executable(glslang::validator IMPORTED)
find_program(GLSLANG_VALIDATOR "glslangValidator" HINTS $ENV{VULKAN_SDK}/bin REQUIRED)
set_property(TARGET glslang::validator PROPERTY IMPORTED_LOCATION ${GLSLANG_VALIDATOR})

# 添加自定义函数，用于编译shader
function(add_shaders_target SHADER_TARGET)

    # 解析参数
    # SHADER_CHAPTER_NAME
    # SHADER_SOURCES
    cmake_parse_arguments("SHADER" "" "CHAPTER_NAME" "SOURCES" ${ARGN})

    set(SHADERS_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${SHADER_CHAPTER_NAME})

    # 创建shader目录
    # add_custom_command(
    #     OUTPUT ${SHADERS_DIR}
    #     COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
    # )
    file(MAKE_DIRECTORY ${SHADERS_DIR})

    # 编译shader
    add_custom_command(
        OUTPUT ${SHADERS_DIR}/frag.spv ${SHADERS_DIR}/vert.spv
        COMMAND glslang::validator
        ARGS --target-env vulkan1.0 ${SHADER_SOURCES} --quiet
        WORKING_DIRECTORY ${SHADERS_DIR}
        DEPENDS ${SHADER_SOURCES}
        COMMENT "compiling shader"
        VERBATIM
    )

    # 添加自定义目标
    add_custom_target(${SHADER_TARGET} DEPENDS ${SHADERS_DIR}/frag.spv ${SHADERS_DIR}/vert.spv)

endfunction()

# 添加自定义函数，用于编译章节
function(add_chapter CHAPTER_NAME)
    # 解析参数
    # CHAPTER_SHADER
    # CHAPTER_LIBS
    # CHAPTER_TEXTURES
    # CHAPTER_MODELS
    cmake_parse_arguments(CHAPTER "" "SHADER" "LIBS;TEXTURES;MODELS" ${ARGN})

    file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${CHAPTER_NAME}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/${CHAPTER_NAME}/*.cpp)

    add_executable(${CHAPTER_NAME} ${SOURCES})

    target_link_libraries(${CHAPTER_NAME} comet)

    # 编译shader
    if (DEFINED CHAPTER_SHADER)

        set(CHAPTER_SHADER_TARGET ${CHAPTER_NAME}_shader)

        file(GLOB SHADER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${CHAPTER_NAME}/${CHAPTER_SHADER}.frag ${CMAKE_CURRENT_SOURCE_DIR}/${CHAPTER_NAME}/${CHAPTER_SHADER}.vert)

        # 使用自定义的函数编译shader
        add_shaders_target(${CHAPTER_SHADER_TARGET} CHAPTER_NAME ${CHAPTER_NAME} SOURCES ${SHADER_SOURCES})

        # 添加依赖顺序，确保shader先编译
        add_dependencies(${CHAPTER_NAME} ${CHAPTER_SHADER_TARGET})

    endif ()

endfunction()

add_chapter(04_logical_device)
# add_chapter(05_window_surface)
# add_chapter(06_swap_chain_creation)
# add_chapter(07_image_views)
# add_chapter(08_graphics_pipeline)
# add_chapter(09_shader_modules SHADER 09_shader_base)
# add_chapter(10_fixed_functions SHADER 10_shader_base)
# add_chapter(11_render_passes SHADER 11_shader_base)
# add_chapter(12_graphics_pipeline_complete SHADER 12_shader_base)
# add_chapter(13_framebuffers SHADER 13_shader_base)
# add_chapter(14_command_buffers SHADER 14_shader_base)
# add_chapter(15_hello_triangle SHADER 15_shader_base)
# add_chapter(16_frames_in_flight SHADER 16_shader_base)
# add_chapter(17_swap_chain_recreation SHADER 17_shader_base)
# add_chapter(18_shader_input SHADER 18_shader_vertex_buffer)

# add_chapter(99_final)
