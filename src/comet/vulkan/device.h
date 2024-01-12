#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "vulkan/vulkan.h"

#include "comet/vulkan/physical_device.h"
#include "comet/vulkan/queue.h"

namespace comet
{
    struct QueueFamilyIndices
    {
        std::optional<unsigned int> graphicsFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value();
        }
    };

    class Device
    {
    public:
        Device(PhysicalDevice physical_device, VkSurfaceKHR surface);
        ~Device();

        VkDevice get_handle() const;

        // void add_queue(size_t global_index, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present);
        // VkQueue get_queue_by_flags(VkQueueFlagBits flags);

        QueueFamilyIndices find_queue_family();

    private:
        const PhysicalDevice &m_physical_device;

        VkSurfaceKHR m_surface;

        VkDevice m_handle;

        std::unique_ptr<Queue> m_graphics_queue;
        std::unique_ptr<Queue> m_present_queue;

        // std::vector<std::vector<Queue>> m_queues;
    };
}