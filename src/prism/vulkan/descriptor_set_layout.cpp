#include "prism/vulkan/descriptor_set_layout.h"

#include "prism/vulkan/error.h"

using namespace prism;

DescriptorSetLayout::DescriptorSetLayout(const Device &device, const Bindings &bindings)
    : m_device(device), m_bindings(bindings)
{
  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = static_cast<uint32_t>(m_bindings.size());
  layout_info.pBindings = m_bindings.data();

  VK_CHECK(vkCreateDescriptorSetLayout(m_device.get_handle(), &layout_info, nullptr, &m_handle));
}

DescriptorSetLayout::~DescriptorSetLayout()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorSetLayout(m_device.get_handle(), m_handle, nullptr);
  }
}

const VkDescriptorSetLayout &DescriptorSetLayout::get_handle() const
{
  return m_handle;
}

std::vector<VkDescriptorPoolSize> DescriptorSetLayout::get_descriptor_pool_sizes() const
{
  std::vector<VkDescriptorPoolSize> pool_sizes;

  for (const auto &binding : m_bindings)
  {
    bool found = false;
    for (auto &pool_size : pool_sizes)
    {
      if (pool_size.type == binding.descriptorType)
      {
        pool_size.descriptorCount += binding.descriptorCount;
        found = true;
        break;
      }
    }

    if (!found)
    {
      VkDescriptorPoolSize pool_size = {};
      pool_size.type = binding.descriptorType;
      pool_size.descriptorCount = binding.descriptorCount;
      pool_sizes.push_back(pool_size);
    }
  }

  return pool_sizes;
}
