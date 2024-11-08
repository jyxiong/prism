#include "prism/vulkan/device.h"

#include "prism/vulkan/error.h"
#include "prism/vulkan/utils.h"

using namespace prism;

Device::Device(const PhysicalDevice &physical_device, const ExtensionNames &extensions, const DeviceFeatures& features)
    : m_physical_device(physical_device)
{

  auto queue_family_count = m_physical_device.get_queue_family_properties().size();
  std::vector<VkDeviceQueueCreateInfo> queue_infos(queue_family_count, {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO});
  std::vector<std::vector<float>> queue_priorities(queue_family_count);

  for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_count; ++queue_family_index)
  {
    const auto &queue_family_property = m_physical_device.get_queue_family_properties()[queue_family_index];

    queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);

    auto &queue_info = queue_infos[queue_family_index];
    queue_info.queueFamilyIndex = queue_family_index;
    queue_info.queueCount = queue_family_property.queueCount;
    queue_info.pQueuePriorities = queue_priorities[queue_family_index].data();
  }

  assert(utils::check_extensions_support(extensions, m_physical_device.get_extensions()));

  VkDeviceCreateInfo device_info{};
  device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_info.queueCreateInfoCount = queue_infos.size();
  device_info.pQueueCreateInfos = queue_infos.data();
  device_info.enabledExtensionCount = extensions.size();
  device_info.ppEnabledExtensionNames = extensions.data();
  device_info.pQueueCreateInfos = queue_infos.data();
  device_info.queueCreateInfoCount = queue_infos.size();
  device_info.pEnabledFeatures = nullptr;
  device_info.pNext = features.data();

  VK_CHECK(vkCreateDevice(m_physical_device.get_handle(), &device_info, nullptr, &m_handle));

  volkLoadDevice(m_handle);

  m_queues.resize(queue_family_count);
  for (uint32_t queue_family_index = 0; queue_family_index < queue_family_count; ++queue_family_index)
  {
    const auto &queue_family_property = m_physical_device.get_queue_family_properties()[queue_family_index];
    auto &queues = m_queues[queue_family_index];
    for (uint32_t queue_index = 0; queue_index < queue_family_property.queueCount; ++queue_index)
    {
      VkQueue queue;
      vkGetDeviceQueue(m_handle, queue_family_index, queue_index, &queue);
      queues.emplace_back(Queue{queue, queue_family_index, queue_index});
    }
  }

  m_extension_functions = std::make_unique<DeviceExtensionFunctions>(this);
}

Device::~Device()
{
  if (m_handle)
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

const Queue &Device::get_queue(uint32_t family_index, uint32_t index) const
{
  return m_queues[family_index][index];
}

const DeviceExtensionFunctions &Device::get_extension_functions() const
{
  return *m_extension_functions;
}


void Device::wait_idle() const
{
  VK_CHECK(vkDeviceWaitIdle(m_handle));
}
