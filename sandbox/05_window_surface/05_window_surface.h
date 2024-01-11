#pragma once

#include <vector>
#include <optional>
#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"

struct QueueFamilyIndices
{
    std::optional<unsigned int> graphicsFamily;
    std::optional<unsigned int> presentFamily;

    [[nodiscard]] bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }

}; // struct QueueFamilyIndices

class HelloTriangleApplication
{
private:
    GLFWwindow *m_window{};
    VkInstance m_instance{};
    VkDebugUtilsMessengerEXT m_debugMessenger{};
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device{};
    VkQueue m_graphicsQueue{};
    VkSurfaceKHR m_surface{};
    VkQueue m_presentQueue{};

public:
    void run();

private:
    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

    //==================================================
    // 初始化vulkan
    //--------------------------------------------------
    // 创建实例
    void createInstance();

    bool checkValidationLayerSupport();

    std::vector<const char *> getRequiredExtensions();

    //--------------------------------------------------
    // 创建调试信息
    void setupDebugMessenger();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    //--------------------------------------------------
    // 选择物理设备
    void pickPhysicalDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    //--------------------------------------------------
    // 创建逻辑设备
    void createLogicalDevice();

    //--------------------------------------------------
    void createSurface();
    //==================================================

}; // class HelloTriangleApplication
