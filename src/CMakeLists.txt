file(GLOB_RECURSE SOURCES comet/*.cpp)
file(GLOB_RECURSE HEADERS comet/*.h)

add_library(comet STATIC ${SOURCES} ${HEADERS})

target_include_directories(comet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(comet PUBLIC volk glfw glm imgui spdlog stb)
