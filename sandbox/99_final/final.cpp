#include <stdexcept>
#include <iostream>
#include <vector>

#include "volk.h"
#include "final.h"

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

HelloTriangleApplication::HelloTriangleApplication()
{
    if(volkInitialize())
    {
        throw std::runtime_error("failed to initialize volk!");
    }
}

HelloTriangleApplication::~HelloTriangleApplication()
{
    m_image_views.clear();

    m_swapchain.reset();

    m_device.reset();

    vkDestroySurfaceKHR(m_instance->get_handle(), m_surface, nullptr);

    m_instance.reset();

    m_window.reset();
}

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
    m_instance = std::make_unique<comet::Instance>("02_validation_layers", getRequiredInstanceExtensions(), validationLayers);

    // 选择physical device
    const auto &physical_device = m_instance->get_suitable_physical_device(nullptr);

    // 创建surface
    m_surface = m_window->create_surface(m_instance->get_handle(), physical_device.get_handle());

    // 创建logical device
    m_device = std::make_unique<comet::Device>(physical_device, m_surface, getRequiredDeviceExtensions());

    // 创建交换链
    m_swapchain = std::make_unique<comet::Swapchain>(*m_device, m_surface);

    // 创建图像视图
    auto& images = m_swapchain->get_images();
    m_image_views.clear();
    // m_image_views.resize(images.size());
    for (size_t i = 0; i < images.size(); i++)
    {
        auto& swapchain_extend = m_swapchain->get_extent();

		VkExtent3D extent{swapchain_extend.width, swapchain_extend.height, 1};

        auto image = comet::Image{*m_device, images[i], extent, m_swapchain->get_format(), m_swapchain->get_usage()};
        m_image_views.emplace_back(image, VK_IMAGE_VIEW_TYPE_2D, m_swapchain->get_format());
    }
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

std::vector<const char *> HelloTriangleApplication::getRequiredInstanceExtensions()
{
    std::vector<const char *> extensions;

    // 获取glfw需要的扩展
    unsigned int glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    extensions.insert(extensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);

    // debug需要的扩展
    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

std::vector<const char *> HelloTriangleApplication::getRequiredDeviceExtensions()
{
    std::vector<const char *> extensions;

    // swapchain需要的扩展
    extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    return extensions;
}