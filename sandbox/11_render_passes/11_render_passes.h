#pragma once

#include <vector>
#include <optional>
#include <string>
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

struct SwapChainSupportDetails
{
    // 分辨率
    VkSurfaceCapabilitiesKHR capabilities;
    // 像素格式、颜色空间
    std::vector<VkSurfaceFormatKHR> formats;
    // 呈现模式
    std::vector<VkPresentModeKHR> presentModes;

}; // struct SwapChainSupportDetails

class HelloTriangleApplication
{
private:
    // GLFW窗口
    GLFWwindow *m_window{};
    // vulkan实例
    VkInstance m_instance{};
    // 调试信息
    VkDebugUtilsMessengerEXT m_debugMessenger{};
    // 物理设备
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    // 逻辑设备
    VkDevice m_device{};
    // 图形队列，跟随逻辑设备自动销毁
    VkQueue m_graphicsQueue{};
    // 呈现队列，跟随逻辑设备自动销毁
    VkQueue m_presentQueue{};
    // 表面
    VkSurfaceKHR m_surface{};
    // 交换链
    VkSwapchainKHR m_swapChain{};
    // 交换链中的图像
    std::vector<VkImage> m_swapChainImages;
    // 交换链中图像的像素格式
    VkFormat m_swapChainImageFormat;
    // 交换链中图像的分辨率
    VkExtent2D m_swapChainExtent;
    // 交换链中的图像视图
    std::vector<VkImageView> m_swapChainImageViews;
    // 渲染通道
    VkRenderPass m_renderPass{};
    // 管线布局
    VkPipelineLayout m_pipelineLayout{};
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

    // 检查是否支持指定的验证层
    bool checkValidationLayerSupport();

    // 获取需要的实例扩展
    std::vector<const char *> getRequiredInstanceExtensions();

    //--------------------------------------------------
    // 创建调试信息
    void setupDebugMessenger();

    // 调试回调函数
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData);

    // 创建调试信息
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    //--------------------------------------------------
    // 选择物理设备
    void pickPhysicalDevice();

    // 检查物理设备是否符合要求：是否支持需要的队列族、是否支持需要的扩展
    bool isDeviceSuitable(VkPhysicalDevice device);

    // 检查物理设备是否支持指定的队列族
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    // 检查物理设备是否支持指定的扩展
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    //--------------------------------------------------
    // 创建逻辑设备
    void createLogicalDevice();

    //--------------------------------------------------
    // 创建窗口表面
    void createSurface();

    //--------------------------------------------------
    // 创建交换链
    void createSwapChain();

    // 获取物理设备支持的交换链信息
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    // 选择交换链的表面格式
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    // 选择交换链的呈现模式
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    // 选择交换链的分辨率
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    //--------------------------------------------------
    // 创建图像视图
    void createImageViews();

    //--------------------------------------------------
    // 创建渲染通道
    void createRenderPass();

    //--------------------------------------------------
    // 创建图形管线
    void createGraphicsPipeline();

    // 读取文件
    static std::vector<char> readFile(const std::string &filename);

    // 创建着色器模块
    VkShaderModule createShaderModule(const std::vector<char> &code);

    //==================================================

}; // class HelloTriangleApplication
