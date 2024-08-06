#pragma once

#include "comet/vulkan/device.h"
#include "comet/vulkan/descriptor_set_layout.h"

namespace comet
{
  class PipelineLayout
  {
  public:
    PipelineLayout(const Device &device, const DescriptorSetLayout &descriptor_set_layout, const std::vector<VkPushConstantRange> &push_constant_ranges = {});
    
    PipelineLayout(const Device &device, const std::vector<DescriptorSetLayout> &descriptor_set_layouts);

    ~PipelineLayout();

    const VkPipelineLayout &get_handle() const;

  private:
    VkPipelineLayout m_handle;

    const Device &m_device;

  }; // class PipelineLayout

} // namespace comet