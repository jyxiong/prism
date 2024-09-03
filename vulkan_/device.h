#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "volk.h"
#include "vk_mem_alloc.h"

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
        Device(const PhysicalDevice &physical_device, VkSurfaceKHR surface, const std::vector<const char *> & required_extensions = {});
        ~Device();

        VkDevice get_handle() const;

        const PhysicalDevice &get_physical_device() const;

	    VmaAllocator get_memory_allocator() const;

        const Queue &get_queue(uint32_t queue_family_index, uint32_t queue_index);

        const Queue &get_queue_by_flags(VkQueueFlags queue_flags, uint32_t queue_index) const;

        const Queue &get_queue_by_present(uint32_t queue_index) const;


        QueueFamilyIndices find_queue_family();

        bool is_extension_supported(const std::string& extension_name) const;

        bool is_extension_enabled(const char *extension_name) const;

    private:
        const PhysicalDevice &m_physical_device;

        VkSurfaceKHR m_surface;

        VkDevice m_handle;

        std::vector<VkExtensionProperties> m_available_extensions;
        std::vector<const char*> m_enabled_extensions;

        std::unique_ptr<Queue> m_graphics_queue;
        std::unique_ptr<Queue> m_present_queue;

        VmaAllocator m_memory_allocator{VK_NULL_HANDLE};

        std::vector<std::vector<Queue>> m_queues;
    };
}