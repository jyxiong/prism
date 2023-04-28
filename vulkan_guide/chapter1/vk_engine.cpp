#include "vk_engine.h"

#include "GLFW/glfw3.h"

#include "VkBootstrap.h"
#include "vk_initializer.h"

#define VK_CHECK(x) \
    do { \
        VkResult err = x; \
        if (err) { \
            printf("Detected Vulkan error: %d", err); \
            abort(); \
        } \
    } while(0) \


void VulkanEngine::init()
{
    // 初始化GLFW
    glfwInit();
    // 不创建OpenGL上下文
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // 窗口大小不可变
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // 创建窗口
    m_window = glfwCreateWindow((int) m_window_extent.width,
                                (int) m_window_extent.height,
                                "Vulkan Engine",
                                nullptr, nullptr);

    init_vulkan();

    init_swap_chain();

    init_command();

    // 初始化完成
    m_is_initialized = true;
}

void VulkanEngine::cleanup()
{
    if (m_is_initialized)
    {
        vkDestroyCommandPool(m_device, m_command_pool, nullptr);

        for (auto &m_image_view: m_image_views)
        {
            vkDestroyImageView(m_device, m_image_view, nullptr);
        }

        vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr);

        vkDestroyDevice(m_device, nullptr);

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger, nullptr);

        vkDestroyInstance(m_instance, nullptr);

        // 销毁窗口
        glfwDestroyWindow(m_window);
        // 终止GLFW
        glfwTerminate();
    }
}

void VulkanEngine::draw()
{

}

void VulkanEngine::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }

    draw();
}

void VulkanEngine::init_vulkan()
{
    vkb::InstanceBuilder inst_builder;
    auto vkb_inst = inst_builder
        .set_app_name("Vulkan Guide") // 设置应用程序名称
        .request_validation_layers(true) // 请求验证层
        .require_api_version(1, 1, 0) // 请求Vulkan 1.1
        .use_default_debug_messenger() // 使用默认的调试消息
        .build() // 构建实例
        .value();
    m_instance = vkb_inst.instance; // 获取Vulkan实例句柄
    m_debug_messenger = vkb_inst.debug_messenger; // 获取调试消息句柄

    // 创建窗口表面
    glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface);

    // 选择物理设备
    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    auto physical_device = selector
        .set_minimum_version(1, 1) // 设置最低Vulkan版本
        .set_surface(m_surface) // 设置窗口表面，表明需要物理设备支持显示功能
        .select() // 选择物理设备
        .value();
    m_physical_device = physical_device.physical_device;

    // 创建逻辑设备
    vkb::DeviceBuilder device_builder{ physical_device };
    auto vkb_device = device_builder
        .build()
        .value();
    m_device = vkb_device.device;

    // 获取队列句柄和队列族索引
    m_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    m_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
}

void VulkanEngine::init_swap_chain()
{
    vkb::SwapchainBuilder swap_chain_builder{ m_physical_device, m_device, m_surface };
    auto swap_chain = swap_chain_builder
        .use_default_format_selection() // 使用默认格式
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // 设置显示模式
        .set_desired_extent(m_window_extent.width, m_window_extent.height) // 设置交换链图像大小
        .build()
        .value();

    m_swap_chain = swap_chain.swapchain;
    m_image_format = swap_chain.image_format;
    m_images = swap_chain.get_images().value();
    m_image_views = swap_chain.get_image_views().value();
}

void VulkanEngine::init_command()
{
    auto command_pool_info = vk_init::command_pool_create_info(m_queue_family);
    VK_CHECK(vkCreateCommandPool(m_device, &command_pool_info, nullptr, &m_command_pool));

    auto command_buffer_info = vk_init::command_buffer_allocate_info(m_command_pool);
    VK_CHECK(vkAllocateCommandBuffers(m_device, &command_buffer_info, &m_command_buffer));
}
