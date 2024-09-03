#include "comet/vulkan/device.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cstring>

using namespace comet;

Device::Device(const PhysicalDevice &physical_device, VkSurfaceKHR surface, const std::vector<const char *> &required_extensions)
    : m_physical_device(physical_device), m_surface(surface)
{
    auto queue_family_properties_count = m_physical_device.get_queue_family_properties().size();
	std::vector<VkDeviceQueueCreateInfo> queue_create_infos(queue_family_properties_count, {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO});
	std::vector<std::vector<float>> queue_priorities(queue_family_properties_count);

    for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_properties_count; ++queue_family_index)
	{
		const VkQueueFamilyProperties &queue_family_property = m_physical_device.get_queue_family_properties()[queue_family_index];

        queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
		

		VkDeviceQueueCreateInfo &queue_create_info = queue_create_infos[queue_family_index];

		queue_create_info.queueFamilyIndex = queue_family_index;
		queue_create_info.queueCount       = queue_family_property.queueCount;
		queue_create_info.pQueuePriorities = queue_priorities[queue_family_index].data();
	}

    // 获取设备支持的扩展
    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(m_physical_device.get_handle(), nullptr, &extension_count, nullptr);
    m_available_extensions.resize(extension_count);
    vkEnumerateDeviceExtensionProperties(m_physical_device.get_handle(), nullptr, &extension_count, m_available_extensions.data());
    if (extension_count > 0)
    {
        for (const auto &extension : m_available_extensions)
        {
            std::cout << "Device extension: " << extension.extensionName << std::endl;
        }
    }

    // 确认必须的扩展是否支持
    for (const auto &required_extension : required_extensions)
    {
        if (!is_extension_supported(required_extension))
        {
            throw std::runtime_error("required extension is not supported!");
        }
        m_enabled_extensions.push_back(required_extension);
    }

    // 创建设备
    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // 添加队列信息
    create_info.queueCreateInfoCount = queue_create_infos.size();
    create_info.pQueueCreateInfos = queue_create_infos.data();
    // 添加设备特性
    VkPhysicalDeviceFeatures device_features{};
    create_info.pEnabledFeatures = &device_features;
    // 校验层
    create_info.enabledLayerCount = 0;
    // 扩展
    create_info.enabledExtensionCount = static_cast<uint32_t>(m_enabled_extensions.size());
    create_info.ppEnabledExtensionNames = m_enabled_extensions.data();

    if (vkCreateDevice(m_physical_device.get_handle(), &create_info, nullptr, &m_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    volkLoadDevice(m_handle);

    m_graphics_queue = std::make_unique<Queue>(*this, find_queue_family().graphicsFamily.value(), m_physical_device.get_queue_family_properties()[find_queue_family().graphicsFamily.value()], m_physical_device.is_present_supported(nullptr, find_queue_family().graphicsFamily.value()), 0);

    m_queues.resize(queue_family_properties_count);
    for (uint32_t queue_family_index = 0; queue_family_index < queue_family_properties_count; ++queue_family_index)
    {
        const auto &queue_family_property = m_physical_device.get_queue_family_properties()[queue_family_index];

        auto support_present = m_physical_device.is_present_supported(nullptr, queue_family_index);
        for (uint32_t queue_index = 0; queue_index < queue_family_property.queueCount; ++queue_index)
        {
            m_queues[queue_family_index].emplace_back(Queue{*this, queue_family_index, queue_family_property, support_present, queue_index});
        }
    }
}

Device::~Device()
{
    vkDestroyDevice(m_handle, nullptr);
}

VkDevice Device::get_handle() const
{
    return m_handle;
}

const PhysicalDevice &Device::get_physical_device() const
{
    return m_physical_device;
}

VmaAllocator Device::get_memory_allocator() const
{
    return m_memory_allocator;
}

const Queue &Device::get_queue(uint32_t queue_family_index, uint32_t queue_index)
{
	return m_queues[queue_family_index][queue_index];
}

const Queue &Device::get_queue_by_flags(VkQueueFlags required_queue_flags, uint32_t queue_index) const
{
	for (uint32_t queue_family_index = 0U; queue_family_index < m_queues.size(); ++queue_family_index)
	{
		Queue const &first_queue = m_queues[queue_family_index][0];

		VkQueueFlags queue_flags = first_queue.get_properties().queueFlags;
		uint32_t     queue_count = first_queue.get_properties().queueCount;

		if (((queue_flags & required_queue_flags) == required_queue_flags) && queue_index < queue_count)
		{
			return m_queues[queue_family_index][queue_index];
		}
	}

	throw std::runtime_error("Queue not found");
}

const Queue &Device::get_queue_by_present(uint32_t queue_index) const
{
	for (uint32_t queue_family_index = 0U; queue_family_index < m_queues.size(); ++queue_family_index)
	{
		Queue const &first_queue = m_queues[queue_family_index][0];

		uint32_t queue_count = first_queue.get_properties().queueCount;

		if (first_queue.support_present() && queue_index < queue_count)
		{
			return m_queues[queue_family_index][queue_index];
		}
	}

	throw std::runtime_error("Queue not found");
}

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

bool Device::is_extension_supported(const std::string& extension_name) const
{
    return std::find_if(m_available_extensions.begin(), m_available_extensions.end(), [&extension_name](const VkExtensionProperties& extension) {
        return extension.extensionName == extension_name;
    }) != m_available_extensions.end();
}

bool Device::is_extension_enabled(const char *extension_name) const
{
    return std::find_if(m_enabled_extensions.begin(), m_enabled_extensions.end(), [extension_name](const char *enabled_extension)
                        { return std::strcmp(extension_name, enabled_extension) == 0; }) != m_enabled_extensions.end();
}