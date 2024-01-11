#include <stdexcept>
#include <iostream>
#include <functional>
#include <vector>

#include <memory>

#include "comet/platform/window/glfw_window.h"
#include "comet/vulkan/instance.h"

/**
 * 创建debug messenger
 * @param instance
 * @param pCreateInfo debug messenger info
 * @param pAllocator
 * @param pDebugMessenger
 * @return
 */
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger
);

/**
 * 销毁debug messenger
 * @param instance
 * @param debugMessenger debug messenger
 * @param pAllocator
 */
void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator);

class HelloTriangleApplication
{
private:
    std::unique_ptr<comet::Window> m_window;
    std::unique_ptr<comet::Instance> m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;

public:
    void run();

private:
    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

    void createInstance();

    std::vector<const char *> getRequiredExtensions();

    /**
     * debug messenger callback function
     * @param messageSeverity
     * @param messageType
     * @param pCallbackData
     * @param pUserData
     * @return
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData);
};
