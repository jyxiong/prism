cmake_minimum_required(VERSION 3.20)

project(learn_vulkan)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    add_compile_options(/wd4819)
endif ()

add_subdirectory(3rdparty)

add_subdirectory(vulkan_tutorial)
add_subdirectory(vulkan_guide)
#add_subdirectory(toy2d)
