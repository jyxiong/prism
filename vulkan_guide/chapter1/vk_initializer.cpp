#include "vk_initializer.h"

namespace vk_init
{

    VkCommandPoolCreateInfo command_pool_create_info(unsigned int queue_family_index, VkCommandPoolCreateFlags flags)
    {
        VkCommandPoolCreateInfo command_pool_info{};
        command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_info.pNext = nullptr;
        command_pool_info.queueFamilyIndex = queue_family_index;
        command_pool_info.flags = flags;

        return command_pool_info;
    }

    VkCommandBufferAllocateInfo
    command_buffer_allocate_info(VkCommandPool command_pool, unsigned int count, VkCommandBufferLevel level)
    {
        VkCommandBufferAllocateInfo command_buffer_info{};
        command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_info.pNext = nullptr;
        command_buffer_info.commandPool = command_pool;
        command_buffer_info.level = level;
        command_buffer_info.commandBufferCount = count;

        return command_buffer_info;
    }

    VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags)
    {
        VkCommandBufferBeginInfo cmd_begin_info = {};
        cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_begin_info.pNext = nullptr;
        cmd_begin_info.pInheritanceInfo = nullptr;
        cmd_begin_info.flags = flags;

        return cmd_begin_info;
    }

    VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass render_pass, VkExtent2D extent, unsigned int layers)
    {
        VkFramebufferCreateInfo framebuffer_info{}; // 帧缓冲信息
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO; // 类型
        framebuffer_info.pNext = nullptr;
        framebuffer_info.renderPass = render_pass; // 渲染通道
        framebuffer_info.attachmentCount = 1; // 附件数量
        framebuffer_info.width = extent.width; // 宽度
        framebuffer_info.height = extent.height; // 高度
        framebuffer_info.layers = layers; // 层数

        return framebuffer_info;
    }

    VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags)
    {
        VkFenceCreateInfo fence_info{}; // 围栏信息
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO; // 类型
        fence_info.pNext = nullptr;
        fence_info.flags = flags; // 创建时已触发

        return fence_info;
    }

    VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags)
    {
        VkSemaphoreCreateInfo semaphore_info{}; // 信号量信息
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO; // 类型
        semaphore_info.pNext = nullptr;
        semaphore_info.flags = flags;

        return semaphore_info;
    }

    VkSubmitInfo submit_info(VkCommandBuffer *command_buffer)
    {
        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = nullptr;
        submit_info.pWaitDstStageMask = nullptr;
        submit_info.waitSemaphoreCount = 0;
        submit_info.pWaitSemaphores = nullptr;
        submit_info.signalSemaphoreCount = 0;
        submit_info.pSignalSemaphores = nullptr;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = command_buffer;

        return submit_info;
    }

    VkPresentInfoKHR present_info()
    {
        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = nullptr;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = nullptr;
        present_info.pImageIndices = nullptr;
        present_info.waitSemaphoreCount = 0;
        present_info.pWaitSemaphores = nullptr;

        return present_info;
    }

    VkRenderPassBeginInfo
    render_pass_begin_info(VkRenderPass render_pass, VkExtent2D window_extent, VkFramebuffer framebuffer)
    {
        VkRenderPassBeginInfo rp_begin_info = {};
        rp_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_begin_info.pNext = nullptr;
        rp_begin_info.renderPass = render_pass;
        rp_begin_info.framebuffer = framebuffer;
        rp_begin_info.renderArea.offset.x = 0;
        rp_begin_info.renderArea.offset.y = 0;
        rp_begin_info.renderArea.extent = window_extent;
        rp_begin_info.clearValueCount = 0;
        rp_begin_info.pClearValues = nullptr;

        return rp_begin_info;
    }
}