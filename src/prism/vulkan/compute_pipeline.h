#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/shader_stage.h"
#include "prism/vulkan/pipeline_layout.h"

namespace prism
{
  class ComputePipeline
  {
  public:
    ComputePipeline(const Device &device, const PipelineLayout &pipeline_layout, const ShaderStage& shader_stage);

    ComputePipeline(const ComputePipeline &) = delete;

    ComputePipeline(ComputePipeline &&other) noexcept;

    ~ComputePipeline();

    ComputePipeline& operator=(const ComputePipeline &) = delete;

    ComputePipeline& operator=(ComputePipeline &&) = delete;

    const VkPipeline &get_handle() const;

  private:
    VkPipeline m_handle;

    const Device& m_device;

  }; // class ComputePipeline

} // namespace prism