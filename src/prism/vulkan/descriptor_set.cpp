#include "prism/vulkan/descriptor_set.h"

#include "prism/vulkan/device.h"

using namespace prism;

DescriptorSet::DescriptorSet(const Device &device, const DescriptorSetLayout& layout, const DescriptorPool& pool)
    : m_device(device), m_layout(layout), m_pool(pool)
{
  VkDescriptorSetAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = m_pool.get_handle();
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &m_layout.get_handle();

  VK_CHECK(vkAllocateDescriptorSets(device.get_handle(), &alloc_info, &m_handle));
}

DescriptorSet::~DescriptorSet()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkFreeDescriptorSets(m_device.get_handle(), m_pool.get_handle(), 1, &m_handle);
  }
}

DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept
    : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
      m_device(other.m_device),
      m_layout(other.m_layout),
      m_pool(other.m_pool)
{
}

const VkDescriptorSet& DescriptorSet::get_handle() const
{
  return m_handle;
}

void DescriptorSet::write(const std::vector<VkDescriptorBufferInfo> &buffer_infos)
{
  std::vector<VkWriteDescriptorSet> writes;
  writes.reserve(buffer_infos.size());

  for (size_t i = 0; i < buffer_infos.size(); i++)
  {
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_handle;
    write.dstBinding = static_cast<uint32_t>(i);
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &buffer_infos[i];

    writes.push_back(write);
  }

  vkUpdateDescriptorSets(m_device.get_handle(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

void DescriptorSet::write(const std::vector<DescriptorBuffer> &buffers, const std::vector<DescriptorImage> &images)
{
  std::vector<VkWriteDescriptorSet> writes(buffers.size() + images.size());

  for (size_t i = 0; i < buffers.size(); i++)
  {
    auto &write = writes[i];
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_handle;
    write.dstBinding = buffers[i].binding;
    write.dstArrayElement = buffers[i].array_element;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &buffers[i].info;
  }

  vkUpdateDescriptorSets(m_device.get_handle(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}