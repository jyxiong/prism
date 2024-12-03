file(GLOB_RECURSE SOURCES prism/*.cpp)
file(GLOB_RECURSE HEADERS prism/*.h)

add_library(prism STATIC)
target_sources(prism PUBLIC ${SOURCES} ${HEADERS})
target_precompile_headers(prism PUBLIC prism/pch.h)
target_include_directories(prism PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(prism PUBLIC Vulkan-Headers glfw glm imgui spdlog stb glslang)
target_compile_definitions(prism PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)