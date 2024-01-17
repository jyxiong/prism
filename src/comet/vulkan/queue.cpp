#include "comet/vulkan/queue.h"

#include "comet/vulkan/device.h"

using namespace comet;

Queue::Queue(Device &device, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present, uint32_t index)
    : m_device(device), m_family_index(family_index), m_properties(properties), m_can_present(can_present), m_index(index)
{
    vkGetDeviceQueue(m_device.get_handle(), m_family_index, m_index, &m_handle);
}

Queue::~Queue()
{
}