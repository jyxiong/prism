#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "02_validation_layers.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// 指定校验层
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

// 是否启用校验层
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void HelloTriangleApplication::run()
{
    initWindow();

    initVulkan();

    mainLoop();

    cleanup();
}

void HelloTriangleApplication::initWindow()
{
    comet::Window::Properties properties;
    properties.extent = {WIDTH, HEIGHT};
    properties.title = "02_validation_layers";
    properties.resizable = false;
    m_window = std::make_unique<comet::GlfwWindow>(properties);
}

void HelloTriangleApplication::initVulkan()
{
    // 创建instance
    createInstance();
}

void HelloTriangleApplication::mainLoop()
{
    while (!m_window->should_close())
    {
        m_window->process_events();
    }
}

void HelloTriangleApplication::cleanup()
{
}

void HelloTriangleApplication::createInstance()
{
    m_instance = std::make_unique<comet::Instance>("02_validation_layers", getRequiredExtensions(), validationLayers);
}

std::vector<const char *> HelloTriangleApplication::getRequiredExtensions()
{
    std::vector<const char *> extensions;

    // 获取glfw需要的扩展
    auto glfw_extensions = m_window->get_required_extensions();
    extensions.insert(extensions.end(), glfw_extensions.begin(), glfw_extensions.end());

    // debug需要的扩展
    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}
