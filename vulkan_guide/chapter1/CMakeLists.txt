file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(chapter1 ${SOURCES})
target_include_directories(chapter0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(chapter1 PUBLIC Vulkan::Vulkan glfw vk-bootstrap VulkanMemoryAllocator)
