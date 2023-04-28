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

}