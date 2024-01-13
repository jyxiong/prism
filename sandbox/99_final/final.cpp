#include <stdexcept>
#include <iostream>
#include <vector>

#include "final.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// 指定校验层
const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

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
    properties.title = "04_logical_device";
    properties.resizable = false;
    m_window = std::make_unique<comet::GlfwWindow>(properties);
}

void HelloTriangleApplication::initVulkan()
{
    // 创建instance
    m_instance = std::make_unique<comet::Instance>("02_validation_layers", getRequiredExtensions(), validationLayers);

    // 选择physical device
    auto physical_device = m_instance->get_suitable_physical_device(nullptr);

    // 创建surface
    auto surface = m_window->create_surface(m_instance->get_handle(), physical_device.get_handle());

    // 创建logical device
    m_device = std::make_unique<comet::Device>(physical_device, surface);

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

std::vector<const char *> HelloTriangleApplication::getRequiredExtensions()
{
    // 获取glfw需要的扩展
    unsigned int glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // debug需要的扩展
    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}
