file(GLOB STB_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h)

add_library(stb INTERFACE ${STB_SOURCES})
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)