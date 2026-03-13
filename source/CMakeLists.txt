file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/prism/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/prism/*.h")

add_library(prism STATIC)
target_sources(prism PUBLIC ${SOURCES} ${HEADERS})
target_include_directories(prism PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(prism PUBLIC volk glfw glm imgui spdlog stb glslang)
