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