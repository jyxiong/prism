#include "comet/vulkan/pipeline_layout.h"

#include "comet/vulkan/error.h"

using namespace comet;

PipelineLayout::PipelineLayout(const Device &device, const DescriptorSetLayout &descriptor_set_layout, const std::vector<VkPushConstantRange> &push_constant_ranges)
    : m_device(device)
{
  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &descriptor_set_layout.get_handle();
  pipeline_layout_info.pushConstantRangeCount = static_cast<uint32_t>(push_constant_ranges.size());
  pipeline_layout_info.pPushConstantRanges = push_constant_ranges.data();

  VK_CHECK(vkCreatePipelineLayout(m_device.get_handle(), &pipeline_layout_info, nullptr, &m_handle));
}

PipelineLayout::PipelineLayout(const Device &device, const std::vector<DescriptorSetLayout> &descriptor_set_layouts)
  : m_device(device)
{
  std::vector<VkDescriptorSetLayout> descriptor_set_layout_handles;
  for (const auto &descriptor_set_layout : descriptor_set_layouts)
  {
    descriptor_set_layout_handles.push_back(descriptor_set_layout.get_handle());
  }

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layout_handles.size());
  pipeline_layout_info.pSetLayouts = descriptor_set_layout_handles.data();
  // TODO: push constant ranges
  pipeline_layout_info.pushConstantRangeCount = 0;
  pipeline_layout_info.pPushConstantRanges = nullptr;

  VK_CHECK(vkCreatePipelineLayout(m_device.get_handle(), &pipeline_layout_info, nullptr, &m_handle));
}

PipelineLayout::~PipelineLayout()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(m_device.get_handle(), m_handle, nullptr);
    m_handle = VK_NULL_HANDLE;
  }
}

const VkPipelineLayout &PipelineLayout::get_handle() const
{
  return m_handle;
}