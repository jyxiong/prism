#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>

#include "03_physical_device_selection.h"

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

    // 选择physical device
    pickPhysicalDevice();
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

void HelloTriangleApplication::pickPhysicalDevice()
{
    const auto& devices = m_instance->get_physical_devices();

    for (const auto &device : devices)
    {
        if (isDeviceSuitable(device))
        {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == nullptr)
        throw std::runtime_error("failed to find a suitable GPU!");
}

bool HelloTriangleApplication::isDeviceSuitable(const std::shared_ptr<comet::PhysicalDevice>& device)
{
    auto indices = findQueueFamilies(device);

    return indices.isComplete();
}

QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(const std::shared_ptr<comet::PhysicalDevice>& device)
{
    QueueFamilyIndices indices;

    auto queueFamilies = device->get_queue_family_properties();

    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        // 检测满足VK_QUEUE_GRAPHICS_BIT的queueFamily
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        // 找到满足需求的queueFamily后退出循环
        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}
