#pragma once

#include <optional>
#include <vector>
#include <filesystem>
#include <array>
#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

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

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    };

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

class HelloTriangleApplication
{

private:
    GLFWwindow *m_window;

    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;

    VkSurfaceKHR m_surface;

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    VkSwapchainKHR m_swapChain;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;

    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;

    VkCommandPool m_commandPool;

    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;

    std::vector<VkCommandBuffer> m_commandBuffers;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    unsigned int m_currentFrame{ 0 };

    bool m_framebufferResized{ false };

public:
    void run();

private:
    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

    //========================
    // 帧缓冲大小改变
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    //------------------------
    // 创建vulkan实例
    void createInstance();

    // 检查是否支持指定的校验层
    bool checkValidationLayersSupport();

    // 获取所需的扩展
    std::vector<const char *> getRequiredExtensions();

    // 创建调试信使
    void setupDebugMessenger();

    // 填充调试信使的创建信息
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    //------------------------
    // 创建窗口表面
    void createSurface();

    //------------------------
    // 选择物理设备
    void pickPhysicalDevice();

    // 检查物理设备是否符合要求
    bool isDeviceSuitable(VkPhysicalDevice device);

    // 要求一：物理设备支持所需的队列族
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    // 要求二：物理设备支持所需的扩展
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    // 要求三：物理设备的窗口表面能够创建交换链
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    //------------------------
    // 创建逻辑设备
    void createLogicalDevice();

    //------------------------
    // 创建交换链
    void createSwapChain();

    // 选择交换链表面格式
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    // 选择交换链显示模式
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    // 选择交换链分辨率
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    //------------------------
    // 创建图像视图
    void createImageViews();

    //------------------------
    // 创建渲染通道
    void createRenderPass();

    //------------------------
    // 创建图形管线
    void createGraphicsPipeline();

    // 读取着色器字节码
    std::vector<char> readFile(const std::filesystem::path &filename);

    // 创建着色器模块
    VkShaderModule createShaderModule(const std::vector<char> &code);

    //------------------------
    // 创建帧缓冲
    void createFramebuffers();

    //------------------------
    // 创建命令池
    void createCommandPool();

    //------------------------
    void createVertexBuffer();

    unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties);

    //------------------------
    // 创建命令缓冲
    void createCommandBuffers();

    //------------------------
    // 记录命令缓冲
    void recordCommandBuffer(VkCommandBuffer commandBuffer, unsigned int imageIndex);

    //------------------------
    // 创建同步
    void createSyncObjects();

    //------------------------
    // 重建交换链
    void recreateSwapChain();

    void cleanupSwapChain();

    //========================
    // 绘制
    void drawFrame();

}; // class HelloTriangleApplication