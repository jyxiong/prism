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