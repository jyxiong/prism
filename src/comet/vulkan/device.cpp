#include "comet/vulkan/device.h"

#include <stdexcept>

using namespace comet;

Device::Device(PhysicalDevice physical_device)
    : m_physical_device(physical_device)
{
    auto queue_family_properties_count = physical_device.get_queue_family_properties().size();

    // 指定队列族
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = find_queue_family().graphicsFamily.value();
    queue_create_info.queueCount = 1;
    float queue_priority = 1.0f;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // 添加队列信息
    create_info.queueCreateInfoCount = 1;
    create_info.pQueueCreateInfos = &queue_create_info;
    // 添加设备特性
    VkPhysicalDeviceFeatures device_features{};
    create_info.pEnabledFeatures = &device_features;
    // 校验层
    create_info.enabledExtensionCount = 0;
    create_info.enabledLayerCount = 0;

    if (vkCreateDevice(physical_device.get_handle(), &create_info, nullptr, &m_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    m_graphics_queue = std::make_unique<Queue>(*this, find_queue_family().graphicsFamily.value(), physical_device.get_queue_family_properties()[find_queue_family().graphicsFamily.value()], physical_device.is_present_supported(nullptr, find_queue_family().graphicsFamily.value()), 0);

    // m_queues.resize(queue_family_properties_count);
    // for (uint32_t queue_family_index = 0; queue_family_index < queue_family_properties_count; ++queue_family_index)
    // {
    //     const auto &queue_family_property = physical_device.get_queue_family_properties()[queue_family_index];

    //     auto support_present = physical_device.is_present_supported(nullptr, queue_family_index);
    //     for (uint32_t queue_index = 0; queue_index < queue_family_property.queueCount; ++queue_index)
    //     {
    //         m_queues[queue_family_index].emplace_back(Queue{*this, queue_family_index, queue_family_property, support_present, queue_index});
    //     }
    // }
}

Device::~Device()
{
    vkDestroyDevice(m_handle, nullptr);
}

VkDevice Device::get_handle() const
{
    return m_handle;
}

// void Device::add_queue(size_t global_index, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present)
// {
//     m_queues[global_index].emplace_back(Queue{*this, family_index, properties, can_present, 0});
// }

QueueFamilyIndices Device::find_queue_family()
{
    QueueFamilyIndices indices;

    auto queue_family_properties = m_physical_device.get_queue_family_properties();

    for (size_t i = 0; i < queue_family_properties.size(); ++i)
    {
        if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }
    }

    return indices;
}