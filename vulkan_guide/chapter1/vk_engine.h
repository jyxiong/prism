#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"
#include "vk_type.h"

class VulkanEngine
{
private:
    bool m_is_initialized{ false };

    VkExtent2D m_window_extent{1280, 960};

    GLFWwindow *m_window{ nullptr };

    // vulkan
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VkPhysicalDevice m_physical_device;
    VkDevice m_device;
    VkSurfaceKHR m_surface;

    // 交换链
    VkSwapchainKHR m_swap_chain;
    VkFormat m_image_format;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_image_views;

    // 命令缓冲
    VkQueue m_queue;
    unsigned int m_queue_family;
    VkCommandPool m_command_pool;
    VkCommandBuffer m_command_buffer;

    // 渲染通道
    VkRenderPass m_render_pass;
    std::vector<VkFramebuffer> m_framebuffers;

public:
    // 初始化引擎
    void init();

    // 清除引擎
    void cleanup();

    // 绘制循环
    void draw();

    // 运行循环
    void run();

private:

    void init_vulkan();

    void init_swap_chain();

    void init_command();

    void init_default_render_pass();

    void init_framebuffers();

}; // class VulkanEngine