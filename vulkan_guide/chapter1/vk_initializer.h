#pragma once

#include "vk_type.h"

namespace vk_init
{
    VkCommandPoolCreateInfo command_pool_create_info(unsigned int queue_family_index,
                                                    VkCommandPoolCreateFlags flags = 0);

    VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool,
                                                             unsigned int count = 1,
                                                             VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}