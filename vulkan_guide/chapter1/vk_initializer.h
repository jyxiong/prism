#pragma once

#include "vk_type.h"

namespace vk_init
{
    VkCommandPoolCreateInfo command_pool_create_info(unsigned int queue_family_index,
                                                     VkCommandPoolCreateFlags flags = 0);

    VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool,
                                                             unsigned int count = 1,
                                                             VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0);

    VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass render_pass,
                                                    VkExtent2D extent,
                                                    unsigned int layers = 1);

    VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);

    VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags = 0);

    VkSubmitInfo submit_info(VkCommandBuffer *command_buffer);

    VkPresentInfoKHR present_info();

    VkRenderPassBeginInfo render_pass_begin_info(VkRenderPass render_pass,
                                                 VkExtent2D window_extent,
                                                 VkFramebuffer framebuffer);
}