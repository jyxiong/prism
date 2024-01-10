file(GLOB SOURCES comet/*.cpp)
file(GLOB HEADERS comet/*.h)

add_library(comet STATIC ${SOURCES} ${HEADERS})

target_include_directories(comet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(comet PUBLIC Vulkan::Vulkan glfw glm imgui spdlog stb)
