#pragma once

#include "comet/vulkan/device.h"
#include "comet/vulkan/shader_module.h"
#include "comet/vulkan/pipeline_layout.h"

namespace comet
{
  class ComputePipeline
  {
  public:
    ComputePipeline(const Device &device, const PipelineLayout &pipeline_layout, const ShaderModule& shader_module);

    ~ComputePipeline();

    const VkPipeline &get_handle() const;

  private:
    VkPipeline m_handle;

    const Device& m_device;

  }; // class ComputePipeline

} // namespace comet