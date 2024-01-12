#include "comet/vulkan/instance.h"

#include <iostream>
#include <cstring>
#include <stdexcept>

using namespace comet;

#ifndef NDEBUG
const bool enable_validation_layers = true;
#else
const bool enable_validation_layers = false;
#endif

VkResult create_debug_utils_messenger_ext(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger);

void destroy_debug_utils_messenger_ext(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks *p_allocator);

bool check_validation_layer_support(const std::vector<const char *> &validation_layers);

void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &create_info);

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
    void *p_user_data);

Instance::Instance(const std::string &app_name, const std::vector<const char *> &extensions, const std::vector<const char *> &validation_layers)
{
    // 检查是否启用校验层
    if (enable_validation_layers)
    {
        // 检查校验层是否支持
        if (!check_validation_layer_support(validation_layers))
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = app_name.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Comet";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    // 创建实例
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &appInfo;

    // 设置扩展
    create_info.enabledExtensionCount = extensions.size();
    create_info.ppEnabledExtensionNames = extensions.data();

    // 设置验证层
    if (enable_validation_layers)
    {
        create_info.enabledLayerCount = validation_layers.size();
        create_info.ppEnabledLayerNames = validation_layers.data();

        // 使用pNext创建单独的debug messenger，记录vkCreateInstance和vkDestroyInstance两个函数的调试信息
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        populate_debug_messenger_create_info(debugCreateInfo);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    }
    else
    {
        create_info.enabledLayerCount = 0;
        create_info.pNext = nullptr;
    }

    if (vkCreateInstance(&create_info, nullptr, &m_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    if (enable_validation_layers)
    {
        // 配置debug messenger的信息
        VkDebugUtilsMessengerCreateInfoEXT create_info{};
        populate_debug_messenger_create_info(create_info);

        if (create_debug_utils_messenger_ext(m_handle, &create_info, nullptr, &m_debug_utils_messenger))
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    query_physical_devices();
}

Instance::~Instance()
{
    if (enable_validation_layers)
    {
        // 销毁debug messenger
        destroy_debug_utils_messenger_ext(m_handle, m_debug_utils_messenger, nullptr);
    }

    // 销毁实例
    vkDestroyInstance(m_handle, nullptr);
}

VkInstance Instance::get_handle() const
{
    return m_handle;
}

const std::vector<std::shared_ptr<PhysicalDevice>> &Instance::get_physical_devices() const
{
    return m_physical_devices;
}

PhysicalDevice &Instance::get_suitable_gpu(VkSurfaceKHR surface)
{
    if (m_physical_devices.empty())
        throw std::runtime_error("failed to find a suitable GPU!");

    // 选择独显
    for (const auto &physical_device : m_physical_devices)
    {
        if (physical_device->get_properties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            // TODO: 检查独显是否支持窗口系统表面
            // 如果是headless模式，surface是nullptr
            return *physical_device;
        }
    }

    throw std::runtime_error("failed to find a suitable GPU!");
}

void Instance::query_physical_devices()
{
    // 获取所有可用的物理设备数量
    unsigned int device_count = 0;
    vkEnumeratePhysicalDevices(m_handle, &device_count, nullptr);

    // 获取所有可用的物理设备
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(m_handle, &device_count, devices.data());

    // 创建物理设备对象
    for (const auto &device : devices)
    {
        m_physical_devices.push_back(std::make_shared<PhysicalDevice>(device));
    }
}

VkResult create_debug_utils_messenger_ext(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger)
{
    // 查找vkCreateDebugUtilsMessengerEXT的函数地址
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, p_create_info, p_allocator, p_debug_messenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_utils_messenger_ext(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks *p_allocator)
{
    // 查找vkDestroyDebugUtilsMessengerEXT的函数地址
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debug_messenger, p_allocator);
    }
}

bool check_validation_layer_support(const std::vector<const char *> &validation_layers)
{
    // 获取所有可用的实例校验层数量
    unsigned int layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    // 获取所有可用的实例校验层
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    // 检查指定的校验层是否包含在可用校验层中
    for (const char *layer_name : validation_layers)
    {
        bool found_layer = false;
        for (const auto &layer_properties : available_layers)
        {
            if (std::strcmp(layer_name, layer_properties.layerName) == 0)
            {
                found_layer = true;
                break;
            }
        }

        if (!found_layer)
        {
            return false;
        }
    }

    return true;
}

void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &create_info)
{
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // 回调函数处理的信息等级
    create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    // 回调函数处理的信息类型
    create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    // 回调函数
    create_info.pfnUserCallback = debug_callback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
    void *p_user_data)
{
    std::cerr << "validation layers" << p_callback_data->pMessage << std::endl;

    return VK_FALSE;
}