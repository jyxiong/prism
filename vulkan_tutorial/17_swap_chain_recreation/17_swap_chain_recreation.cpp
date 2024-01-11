#include "17_swap_chain_recreation.h"

#include <stdexcept>
#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <algorithm>
#include "GLFW/glfw3.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const unsigned int MAX_FRAMES_IN_FLIGHT = 2;

// 指定实例支持的校验层
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

// 指定设备支持的扩展
const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    // 获取函数指针
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                            "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
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
    glfwInit();
    // 不创建OpenGL上下文
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
}

void HelloTriangleApplication::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto app = reinterpret_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
    app->m_framebufferResized = true;
}

void HelloTriangleApplication::initVulkan()
{
    createInstance();

    setupDebugMessenger();

    createSurface();

    pickPhysicalDevice();

    createLogicalDevice();

    createSwapChain();

    createImageViews();

    createRenderPass();

    createGraphicsPipeline();

    createFramebuffers();

    createCommandPool();

    createCommandBuffers();

    createSyncObjects();
}

void HelloTriangleApplication::mainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(m_device);
}

void HelloTriangleApplication::cleanup()
{
    cleanupSwapChain();

    // 销毁同步对象
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
    }

    // 销毁命令池
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);

    // 销毁图形管线
    vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);

    // 销毁管线布局
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

    // 销毁渲染通道
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);

    // 销毁逻辑设备
    vkDestroyDevice(m_device, nullptr);

    // 销毁debug messenger
    if (enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    // 销毁surface
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    // 销毁实例
    vkDestroyInstance(m_instance, nullptr);

    // 销毁窗口
    glfwDestroyWindow(m_window);

    // 终止GLFW
    glfwTerminate();
}

void HelloTriangleApplication::createInstance()
{
    // 检查指定的验证层是否可用
    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    // 不使用引擎
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    // API版本
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // 实例
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    // 实例的应用信息
    createInfo.pApplicationInfo = &appInfo;
    // 实例的扩展信息
    auto extensions = getRequiredInstanceExtensions();
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    // 实例的验证层信息
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
        // 在创建实例和销毁实例时，调用调试信息回调函数
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    } else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }
    // 创建实例
    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

bool HelloTriangleApplication::checkValidationLayerSupport()
{
    // 获取可用的校验层
    unsigned int layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // 检查指定的校验层是否在可用的校验层中
    for (const char *layerName: validationLayers)
    {
        bool layerFound = false;
        for (const auto &layerProperties: availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) return false;
    }

    return true;
}

std::vector<const char *> HelloTriangleApplication::getRequiredInstanceExtensions()
{
    // glfw需要的扩展
    unsigned int glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // 仅仅启用校验层没用，还需要debug扩展来获取调试信息
    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void HelloTriangleApplication::setupDebugMessenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkBool32 HelloTriangleApplication::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                 VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                 const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                 void *pUserData)
{
    // 返回调用信息
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void HelloTriangleApplication::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void HelloTriangleApplication::createSurface()
{
    if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

void HelloTriangleApplication::pickPhysicalDevice()
{
    // 获取可用的GPU
    unsigned int deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    // 选择一个合适的GPU
    for (const auto &device: devices)
    {
        if (isDeviceSuitable(device))
        {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool HelloTriangleApplication::isDeviceSuitable(VkPhysicalDevice device)
{
    // 检查物理设备是否支持指定的队列族
    auto indices = findQueueFamilies(device);

    // 检查物理设备是否支持指定的扩展
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    // 获取物理设备支持的交换链信息
    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    // 获取物理设备的队列族
    unsigned int queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // 查找队列族
    int i = 0;
    for (const auto &queueFamily: queueFamilies)
    {
        // 支持VK_QUEUE_GRAPHICS_BIT的队列族
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        // 支持显示的队列族
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        // 找到了就退出
        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

bool HelloTriangleApplication::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    // 获取可用的扩展
    unsigned int extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    // 检查是否支持需要的扩展
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto &extension: availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
}

void HelloTriangleApplication::createLogicalDevice()
{
    // 查找队列族
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<unsigned int> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    // 创建设备队列
    float queuePriority = 1.0f;
    for (unsigned int queueFamily: uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        // 指定队列族
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        // 指定队列的优先级
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // 设备功能
    VkPhysicalDeviceFeatures deviceFeatures{};

    // 创建逻辑设备
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // 指定设备队列信息
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    // 指定设备的功能
    createInfo.pEnabledFeatures = &deviceFeatures;
    // 指定设备的扩展
    createInfo.enabledExtensionCount = deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    // 指定设备的校验层
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else
    {
        createInfo.enabledLayerCount = 0;
    }
    // 创建逻辑设备
    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    // 分别获取图形队列和显示队列
    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

void HelloTriangleApplication::createSwapChain()
{
    // 获取交换链信息
    auto swapChainSupport = querySwapChainSupport(m_physicalDevice);
    auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    auto extent = chooseSwapExtent(swapChainSupport.capabilities);

    // 交换链中的图像数量
    unsigned int imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // 创建交换链信息
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.presentMode = presentMode;
    createInfo.imageExtent = extent;
    createInfo.minImageCount = imageCount;
    // 总是1，除非是立体3D图像
    createInfo.imageArrayLayers = 1;
    // 交换链中的图像用于颜色附件
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // 获取队列族，判断图形队列和显示队列不是同一个队列
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
    unsigned int queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    if (indices.graphicsFamily != indices.presentFamily)
    {
        // 图像共享模式
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else
    {
        // 图像独占模式
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    // 不进行任何变换
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    // 指定alpha通道是否用于混合
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    // 指定是否要裁剪窗口外的像素
    createInfo.clipped = VK_TRUE;

    // 交换链失效处理
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // 创建交换链
    if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    // 获取交换链中的图像、格式和分辨率
    vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}

SwapChainSupportDetails HelloTriangleApplication::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    // 获取物理设备表面分辨率
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    // 获取物理设备表面格式
    unsigned int formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
    }

    // 获取物理设备表面呈现模式
    unsigned int presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR HelloTriangleApplication::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    // 遍历可用的表面格式列表，寻找指定的表面格式返回
    for (const auto &availableFormat: availableFormats)
    {
        // 如果可用的表面格式与指定的表面格式相同，则指定表面格式
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    // 如果没有找到指定的表面格式，则返回第一个可用的表面格式
    return availableFormats[0];
}

VkPresentModeKHR
HelloTriangleApplication::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    // 便利可用的呈现模式列表，寻找指定的表面呈现模式，返回
    for (const auto &availablePresentMode: availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    // 如果没有找到指定的表面呈现模式，则返回 VK_PRESENT_MODE_FIFO_KHR
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D HelloTriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    // 如果当前表面分辨率不等于std::max()，表明不允许指定分辨率，返回当前表面分辨率
    if (capabilities.currentExtent.width != std::numeric_limits<unsigned int>::max())
    {
        return capabilities.currentExtent;
    } else
    {
        // 获取窗口的宽度和高度
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<unsigned int>(width),
            static_cast<unsigned int>(height)
        };

        // 限制交换链图像的分辨率在 minImageExtent 和 maxImageExtent 之间
        actualExtent.width = std::clamp(actualExtent.width,
                                        capabilities.minImageExtent.width,
                                        capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                         capabilities.minImageExtent.height,
                                         capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void HelloTriangleApplication::createImageViews()
{
    // 分配图像视图空间
    m_swapChainImageViews.resize(m_swapChainImages.size());
    // 便利交换链图像，创建图像视图
    for (unsigned int i = 0; i < m_swapChainImages.size(); i++)
    {
        // 创建图像视图信息
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_swapChainImageFormat;
        // 设置图像视图的通道映射
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // 设置图像视图的子资源范围
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // 图像的mipmap层级
        createInfo.subresourceRange.baseMipLevel = 0;
        // 图像的mipmap层级数量
        createInfo.subresourceRange.levelCount = 1;
        // 图像的数组层级，用于三维图像
        createInfo.subresourceRange.baseArrayLayer = 0;
        // 图像的数组层级数量
        createInfo.subresourceRange.layerCount = 1;

        // 创建图像视图
        if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void HelloTriangleApplication::createRenderPass()
{
    // 创建颜色附件描述信息
    VkAttachmentDescription colorAttachment{};
    // 附件的格式
    colorAttachment.format = m_swapChainImageFormat;
    // 附件的采样数(MSAA)
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // 附件的加载操作，用于颜色和深度附件
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // 附件的存储操作，用于颜色和深度附件
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // 附件的加载操作，用于模板附件
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // 附件的存储操作，用于模板附件
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // 附件的初始布局
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // 附件的最终布局
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // 创建颜色附件引用
    VkAttachmentReference colorAttachmentRef{};
    // 附件的索引
    colorAttachmentRef.attachment = 0;
    // 附件的布局
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // 创建渲染子过程描述信息
    VkSubpassDescription subpass{};
    // 渲染子过程的类型
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    // 渲染子过程的颜色附件引用
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // 创建子过程依赖
    VkSubpassDependency dependency{};
    // 依赖的源子过程索引
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    // 依赖的目标子过程索引
    dependency.dstSubpass = 0;
    // 依赖的源阶段掩码
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // 依赖的源访问掩码
    dependency.srcAccessMask = 0;
    // 依赖的目标阶段掩码
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // 依赖的目标访问掩码
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // 创建渲染过程描述信息
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // 渲染过程的颜色附件描述信息
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    // 渲染过程的渲染子过程描述信息
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    // 渲染过程的子过程依赖
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    // 创建渲染过程
    if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

void HelloTriangleApplication::createGraphicsPipeline()
{
    // 读取着色器文件
    auto vertShaderCode = readFile("../shaders/vert.spv");
    auto fragShaderCode = readFile("../shaders/frag.spv");

    // 创建着色器模块
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    // 创建顶点着色器阶段信息
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    // 创建片段着色器阶段信息
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    // 着色器阶段信息列表
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // 顶点输入阶段信息
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    // 输入装配阶段信息
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // 视口和裁剪信息
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) m_swapChainExtent.width;
    viewport.height = (float) m_swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // 视口信息
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    // 裁剪信息
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // 光栅化阶段信息
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // 指定是否clamp深度值，否则丢弃深度值超出范围的片段
    rasterizer.depthClampEnable = VK_FALSE;
    // 指定是否禁止光栅化阶段输出图元
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    // 指定图元的填充模式
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    // 指定面剔除模式
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    // 指定顶点的绘制顺序
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // 指定是否开启深度偏移
    rasterizer.depthBiasEnable = VK_FALSE;

    // 多重采样阶段信息
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    // 指定是否开启多重采样
    multisampling.sampleShadingEnable = VK_FALSE;
    // 指定多重采样的采样级别
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    // 深度模板测试阶段信息

    // 颜色混合阶段信息方式一：单独指定每个颜色附件的混合状态
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    // 指定颜色混合附件的通道
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // 指定是否开启颜色混合
    colorBlendAttachment.blendEnable = VK_FALSE;
    // 指定颜色混合因子
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    // 指定颜色混合操作
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    // 指定alpha混合因子
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    // 指定alpha混合操作
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    // 颜色混合阶段信息方式二：指定全局的混合状态
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    // 指定是否开启全局颜色混合
    colorBlending.logicOpEnable = VK_FALSE;
    // 指定逻辑操作
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    // 指定混合附件的数量
    colorBlending.attachmentCount = 1;
    // 指定混合附件的状态
    colorBlending.pAttachments = &colorBlendAttachment;
    // 指定混合常量
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // 动态状态：指定可以在绘制命令中修改的状态
    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    // 创建管线布局
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // 创建图形管线
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // 指定管线阶段的数量
    pipelineInfo.stageCount = 2;
    // 指定管线阶段的信息
    pipelineInfo.pStages = shaderStages;
    // 指定顶点输入阶段的信息
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    // 指定输入装配阶段的信息
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    // 指定视口和裁剪矩形阶段的信息
    pipelineInfo.pViewportState = &viewportState;
    // 指定光栅化阶段的信息
    pipelineInfo.pRasterizationState = &rasterizer;
    // 指定多重采样阶段的信息
    pipelineInfo.pMultisampleState = &multisampling;
    // 指定深度模板测试阶段的信息
    pipelineInfo.pDepthStencilState = nullptr;
    // 指定颜色混合阶段的信息
    pipelineInfo.pColorBlendState = &colorBlending;
    // 指定动态状态的信息
    pipelineInfo.pDynamicState = &dynamicState;
    // 指定管线布局
    pipelineInfo.layout = m_pipelineLayout;
    // 指定渲染目标
    pipelineInfo.renderPass = m_renderPass;
    // 指定子管线索引
    pipelineInfo.subpass = 0;
    // 指定基础管线句柄
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    // 指定基础管线索引
    pipelineInfo.basePipelineIndex = -1;
    // 创建图形管线
    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // 销毁着色器模块
    vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
}

std::vector<char> HelloTriangleApplication::readFile(const std::string &filename)
{
    // 打开文件：从尾部开始读取，二进制格式
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    // 从尾部开始读取文件，能够获取文件大小
    auto fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    // 从文件头开始读取文件内容
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule HelloTriangleApplication::createShaderModule(const std::vector<char> &code)
{
    // 创建着色器模块信息
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    // 着色器代码的字节码指针类型转换
    createInfo.pCode = reinterpret_cast<const unsigned int *>(code.data());

    // 创建着色器模块
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void HelloTriangleApplication::createFramebuffers()
{
    m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = {m_swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_swapChainExtent.width;
        framebufferInfo.height = m_swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void HelloTriangleApplication::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // 指定命令池分配的命令缓冲区，对应的队列族
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    // 指令池分配的命令缓冲区互相独立
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    // 创建指令池
    if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

void HelloTriangleApplication::createCommandBuffers()
{
    m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    // 指定指令池
    allocInfo.commandPool = m_commandPool;
    // 指定为主命令缓冲区
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = m_commandBuffers.size();

    if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void HelloTriangleApplication::createSyncObjects()
{
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    // 创建semaphore对象
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // 创建fence对象
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

}

void HelloTriangleApplication::drawFrame()
{
    // 等待Fence完毕
    vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

    // 获取当前可用的交换链图像
    unsigned int imageIndex;
    auto result = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame],
                                        VK_NULL_HANDLE,
                                        &imageIndex);

    // 交换链图像已经过期，需要重新创建交换链
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // 重置fence
    vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);

    // 重置命令缓冲区
    vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);
    // 记录命令缓冲区
    recordCommandBuffer(m_commandBuffers[m_currentFrame], imageIndex);

    // 提交命令缓冲区
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    // 等待信号量
    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    // 等待管线阶段
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.pWaitDstStageMask = waitStages;
    // 提交的命令缓冲区
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
    // 信号量
    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // 呈现信息
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    // 等待信号量
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    // 交换链
    VkSwapchainKHR swapChains[] = { m_swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    // 交换链图像索引
    presentInfo.pImageIndices = &imageIndex;
    // 提交队列
    result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
    {
        m_framebufferResized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    // 更新帧索引
    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void HelloTriangleApplication::recordCommandBuffer(VkCommandBuffer commandBuffer, unsigned int imageIndex)
{
    // 开始记录命令
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // 渲染流程信息
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapChainExtent;
    // 清除颜色
    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    // 开始渲染流程
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // 绑定图形管线
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
    {
        // 动态视口
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) m_swapChainExtent.width;
        viewport.height = (float) m_swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        // 动态裁剪
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    // 绘制
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    // 结束渲染流程
    vkCmdEndRenderPass(commandBuffer);

    // 结束记录命令
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void HelloTriangleApplication::recreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(m_window, &width, &height);
        glfwWaitEvents();
    }

    // 等待GPU执行完毕
    vkDeviceWaitIdle(m_device);

    // 清理交换链
    cleanupSwapChain();

    // 重建交换链
    createSwapChain();
    // 重建交换链图像视图
    createImageViews();
    // 重建帧缓冲区
    createFramebuffers();
}

void HelloTriangleApplication::cleanupSwapChain()
{
    for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(m_device, m_swapChainFramebuffers[i], nullptr);
    }

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
    {
        vkDestroyImageView(m_device, m_swapChainImageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
}
