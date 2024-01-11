#include <stdexcept>
#include <iostream>
#include <functional>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
    GLFWwindow *m_window;
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;

public:
    void run();

private:
    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

    void createInstance();

    bool checkValidationLayerSupport();

    std::vector<const char *> getRequiredExtensions();

    /**
     *
     * @param createInfo
     */
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    /**
     * 创建debug messenger
     */
    void setupDebugMessenger();

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
