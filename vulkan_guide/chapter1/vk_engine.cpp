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

    init_default_render_pass();

    init_framebuffers();

    init_sync_structures();

    // 初始化完成
    m_is_initialized = true;
}

void VulkanEngine::cleanup()
{
    if (m_is_initialized)
    {
        vkDeviceWaitIdle(m_device);

        vkDestroySemaphore(m_device, m_render_semaphore, nullptr);
        vkDestroySemaphore(m_device, m_present_semaphore, nullptr);
        vkDestroyFence(m_device, m_render_fence, nullptr);

        vkDestroyCommandPool(m_device, m_command_pool, nullptr);

        vkDestroyRenderPass(m_device, m_render_pass, nullptr);

        for (unsigned int i = 0; i < m_framebuffers.size(); ++i)
        {
            vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);

            vkDestroyImageView(m_device, m_image_views[i], nullptr);
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
    // 等待GPU
    VK_CHECK(vkWaitForFences(m_device, 1, &m_render_fence, true, UINT64_MAX));
    VK_CHECK(vkResetFences(m_device, 1, &m_render_fence));

    // 重置命令缓冲
    VK_CHECK(vkResetCommandBuffer(m_command_buffer, 0));

    // 获取图像
    uint32_t image_index;
    VK_CHECK(vkAcquireNextImageKHR(m_device,
                                   m_swap_chain,
                                   UINT64_MAX,
                                   m_present_semaphore,
                                   nullptr,
                                   &image_index));


    // 开始命令缓冲
    auto cmd_begin_info = vk_init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VK_CHECK(vkBeginCommandBuffer(m_command_buffer, &cmd_begin_info));

    // 背景颜色
    VkClearValue clear_value;
    float flash = fabsf(sinf((float) m_frame_number / 120.f));
    clear_value.color = { 0.0f, 0.0f, flash, 1.0f };

    // 渲染通道
    auto rp_begin_info = vk_init::render_pass_begin_info(m_render_pass, m_window_extent, m_framebuffers[image_index]);
    rp_begin_info.clearValueCount = 1;
    rp_begin_info.pClearValues = &clear_value;

    // 开始渲染通道
    vkCmdBeginRenderPass(m_command_buffer, &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    // 结束渲染通道
    vkCmdEndRenderPass(m_command_buffer);

    // 结束命令缓冲
    VK_CHECK(vkEndCommandBuffer(m_command_buffer));

    // 提交命令缓冲
    auto submit_info = vk_init::submit_info(&m_command_buffer);
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submit_info.pWaitDstStageMask = &wait_stage;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &m_present_semaphore;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &m_render_semaphore;
    VK_CHECK(vkQueueSubmit(m_queue, 1, &submit_info, m_render_fence));

    // 提交图像
    auto present_info = vk_init::present_info();
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &m_swap_chain;
    present_info.pImageIndices = &image_index;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &m_render_semaphore;
    VK_CHECK(vkQueuePresentKHR(m_queue, &present_info));

    ++m_frame_number;
}

void VulkanEngine::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();

        draw();
    }
}

void VulkanEngine::init_vulkan()
{
    // 创建Vulkan实例
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
    auto command_pool_info = vk_init::command_pool_create_info(m_queue_family,
                                                               VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VK_CHECK(vkCreateCommandPool(m_device, &command_pool_info, nullptr, &m_command_pool));

    auto command_buffer_info = vk_init::command_buffer_allocate_info(m_command_pool);
    VK_CHECK(vkAllocateCommandBuffers(m_device, &command_buffer_info, &m_command_buffer));
}

void VulkanEngine::init_default_render_pass()
{
    VkAttachmentDescription color_attachment{}; // 颜色附件
    color_attachment.format = m_image_format; // 图像格式
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT; // 采样数
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // 清除颜色
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // 保存颜色
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 不关注模板
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 不关注模板
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // 不关注之前的图像布局
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // 最终图像布局为显示

    VkAttachmentReference color_attachment_ref{}; // 颜色附件引用
    color_attachment_ref.attachment = 0; // 引用附件索引
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // 图像布局

    VkSubpassDescription subpass{}; // 子通道
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // 绑定点
    subpass.colorAttachmentCount = 1; // 颜色附件数量
    subpass.pColorAttachments = &color_attachment_ref; // 颜色附件引用

    VkRenderPassCreateInfo render_pass_info{}; // 渲染通道信息
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO; // 类型
    render_pass_info.attachmentCount = 1; // 附件数量
    render_pass_info.pAttachments = &color_attachment; // 附件
    render_pass_info.subpassCount = 1; // 子通道数量
    render_pass_info.pSubpasses = &subpass; // 子通道

    VK_CHECK(vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass));
}

void VulkanEngine::init_framebuffers()
{
    auto framebuffer_info = vk_init::framebuffer_create_info(m_render_pass, m_window_extent); // 帧缓冲信息
    framebuffer_info.attachmentCount = 1; // 附件数量

    m_framebuffers.resize(m_image_views.size()); // 调整帧缓冲数量
    for (size_t i = 0; i < m_framebuffers.size(); ++i)
    {
        framebuffer_info.pAttachments = &m_image_views[i]; // 附件
        VK_CHECK(vkCreateFramebuffer(m_device, &framebuffer_info, nullptr, &m_framebuffers[i]));
    }
}

void VulkanEngine::init_sync_structures()
{
    auto fence_info = vk_init::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT); // 围栏信息
    VK_CHECK(vkCreateFence(m_device, &fence_info, nullptr, &m_render_fence));

    auto semaphore_info = vk_init::semaphore_create_info(); // 信号量信息
    VK_CHECK(vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_present_semaphore));
    VK_CHECK(vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_render_semaphore));
}
