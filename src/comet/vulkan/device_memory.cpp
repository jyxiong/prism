#include "comet/vulkan/device_memory.h"

#include <memory>
#include <stdexcept>

#include "comet/vulkan/error.h"

using namespace comet;

DeviceMemory::DeviceMemory(const Device &device, const VkMemoryRequirements &requirements, VkMemoryPropertyFlags property_flags, VkMemoryAllocateFlags allocate_flags)
    : m_device(device), m_requirements(requirements), m_property_flags(property_flags)
{
    uint32_t memory_type_bits = m_requirements.memoryTypeBits;

    const auto &memory_properties = device.get_physical_device().get_memory_properties();

    uint32_t memory_type_index = memory_properties.memoryTypeCount;
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
    {
        if ((memory_type_bits & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & m_property_flags) == m_property_flags)
        {
            memory_type_index = i;
            break;
        }
    }

    if (memory_type_index == memory_properties.memoryTypeCount)
    {
        throw std::runtime_error("Failed to find suitable memory type");
    }

    VkMemoryAllocateFlagsInfo flags_info = {};
    flags_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    flags_info.flags = allocate_flags;
    
    VkMemoryAllocateInfo allocate_info;
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.pNext = &flags_info;
    allocate_info.allocationSize = m_requirements.size;
    allocate_info.memoryTypeIndex = memory_type_index;

    VK_CHECK(vkAllocateMemory(m_device.get_handle(), &allocate_info, nullptr, &m_handle));
}

DeviceMemory::~DeviceMemory()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkFreeMemory(m_device.get_handle(), m_handle, nullptr);
    }
}

void DeviceMemory::copy(VkDeviceSize offset, VkDeviceSize size, const void *src_data)
{
    void *dst_data;
    map(offset, size, 0, &dst_data);
    std::memcpy(dst_data, src_data, size);
    unmap();
}

void DeviceMemory::map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **pp_data)
{
    VK_CHECK(vkMapMemory(m_device.get_handle(), m_handle, offset, size, flags, pp_data));
}

void DeviceMemory::unmap()
{
    vkUnmapMemory(m_device.get_handle(), m_handle);
}

VkDeviceMemory DeviceMemory::get_handle() const
{
    return m_handle;
}

const Device& DeviceMemory::get_device() const
{
    return m_device;
}

VkMemoryRequirements DeviceMemory::get_requirements() const
{
    return m_requirements;
}

VkMemoryPropertyFlags DeviceMemory::get_properties() const
{
    return m_property_flags;
}
