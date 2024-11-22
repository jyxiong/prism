#include "prism/vulkan/descriptor_pool.h"

using namespace prism;

DescriptorPool::DescriptorPool(const Device &device, const std::vector<VkDescriptorPoolSize> &pool_sizes, uint32_t max_sets)
    : m_device(device), m_max_sets(max_sets), m_sizes(pool_sizes)
{
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = static_cast<uint32_t>(m_sizes.size());
  pool_info.pPoolSizes = m_sizes.data();
  pool_info.maxSets = m_max_sets;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

  VK_CHECK(vkCreateDescriptorPool(m_device.get_handle(), &pool_info, nullptr, &m_handle));
}

DescriptorPool::DescriptorPool(DescriptorPool &&other) noexcept
    : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
      m_device(other.m_device),
      m_max_sets(other.m_max_sets),
      m_sizes(std::move(other.m_sizes))
{
}

DescriptorPool::~DescriptorPool()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorPool(m_device.get_handle(), m_handle, nullptr);
  }
}

VkDescriptorSet DescriptorPool::allocate(const VkDescriptorSetLayout &layout)
{
  VkDescriptorSetAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = m_handle;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &layout;

  VkDescriptorSet descriptor_set;
  VK_CHECK(vkAllocateDescriptorSets(m_device.get_handle(), &alloc_info, &descriptor_set));

  return descriptor_set;
}

void DescriptorPool::free(const VkDescriptorSet &descriptor_set)
{
  VK_CHECK(vkFreeDescriptorSets(m_device.get_handle(), m_handle, 1, &descriptor_set));
}

VkDescriptorPool DescriptorPool::get_handle() const
{
  return m_handle;
}