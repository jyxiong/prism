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

    set(SHADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_CHAPTER_NAME}/shaders)

    # 创建shader目录
    add_custom_command(
        OUTPUT ${SHADERS_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
    )

    # 编译shader
    add_custom_command(
        OUTPUT ${SHADERS_DIR}/frag.spv ${SHADERS_DIR}/vert.spv
        COMMAND glslang::validator
        ARGS --target-env vulkan1.0 ${SHADER_SOURCES} --quiet
        WORKING_DIRECTORY ${SHADERS_DIR}
        DEPENDS ${SHADER_SOURCES}
        COMMENT "Compiling Shaders"
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

    set_target_properties(${CHAPTER_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER_NAME})

    set_target_properties(${CHAPTER_NAME} PROPERTIES CXX_STANDARD 17)

    target_link_libraries(${CHAPTER_NAME} Vulkan::Vulkan glfw)

    target_include_directories(${CHAPTER_NAME} PRIVATE ${STB_INCLUDEDIR})

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

add_chapter(00_base_code)
add_chapter(01_instance_creation)
add_chapter(02_validation_layers)
add_chapter(03_physical_device_selection)
add_chapter(04_logical_device)
add_chapter(05_window_surface)
add_chapter(06_swap_chain_creation)
add_chapter(07_image_views)
add_chapter(08_graphics_pipeline)
add_chapter(09_shader_modules SHADER 09_shader_base)
add_chapter(10_fixed_functions SHADER 10_shader_base)
add_chapter(11_render_passes SHADER 11_shader_base)
