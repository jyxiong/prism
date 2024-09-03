file(GLOB_RECURSE SOURCES prism/*.cpp)
file(GLOB_RECURSE HEADERS prism/*.h)

add_library(prism STATIC ${SOURCES} ${HEADERS})
target_include_directories(prism PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(prism PUBLIC volk glfw glm imgui spdlog stb)
target_precompile_headers(prism PUBLIC prism/pch.h)