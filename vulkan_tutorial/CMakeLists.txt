function(add_chapter CHAPTER_NAME)

    file(GLOB SOURCES ./${CHAPTER_NAME}/*.h ./${CHAPTER_NAME}/*.cpp)
    add_executable(${CHAPTER_NAME} ${SOURCES})
    set_target_properties(${CHAPTER_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CHAPTER_NAME})
    set_target_properties(${CHAPTER_NAME} PROPERTIES CXX_STANDARD 17)
    target_link_libraries(${CHAPTER_NAME} Vulkan::Vulkan glfw)
    target_include_directories(${CHAPTER_NAME} PRIVATE ${STB_INCLUDEDIR})

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
add_chapter(09_shader_modules)