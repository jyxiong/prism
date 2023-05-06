#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"

struct QueueFamilyIndices
{
    std::optional<unsigned int> graphicsFamily;
    std::optional<unsigned int> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class HelloTriangleApplication
{

private:
    GLFWwindow *m_window;

    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    VkSurfaceKHR m_surface;

public:
    void run();

private:
    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

    //========================
    // initVulkan
    //------------------------
    void createInstance();
    //------------------------
    bool checkValidationLayersSupport();
    //------------------------
    std::vector<const char *> getRequiredExtensions();
    //------------------------
    void setupDebugMessenger();
    //------------------------
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    //------------------------
    void pickPhysicalDevice();
    //------------------------
    bool isDeviceSuitable(VkPhysicalDevice device);
    //------------------------
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    //------------------------
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    //------------------------
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    //------------------------
    void createLogicalDevice();
    //------------------------
    void createSurface();

}; // class HelloTriangleApplication