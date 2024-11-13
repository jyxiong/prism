#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/graphics_pipeline_state.h"
#include "prism/vulkan/pipeline_layout.h"
#include "prism/vulkan/render_pass.h"
#include "prism/vulkan/shader_stage.h"

namespace prism
{
  struct GraphicsPipelineCreateInfo : public VkGraphicsPipelineCreateInfo
  {
      GraphicsPipelineCreateInfo();

      GraphicsPipelineCreateInfo& set_shader_stages(const std::vector<ShaderStage> &shader_stages);

      GraphicsPipelineCreateInfo& set_vertex_input_state(const VertexInputState &vertex_input_state);

      GraphicsPipelineCreateInfo& set_input_assembly_state(const InputAssemblyState &input_assembly_state);

      GraphicsPipelineCreateInfo& set_viewport_state(const ViewportState &viewport_state);

      GraphicsPipelineCreateInfo& set_rasterization_state(const RasterizationState &rasterization_state);

      GraphicsPipelineCreateInfo& set_multisample_state(const MultisampleState &multisample_state);

      GraphicsPipelineCreateInfo& set_depth_stencil_state(const DepthStencilState &depth_stencil_state);

      GraphicsPipelineCreateInfo& set_color_blend_state(const ColorBlendState &color_blend_state);

      GraphicsPipelineCreateInfo& set_dynamic_state(const DynamicState &dynamic_state);

      GraphicsPipelineCreateInfo& set_tesellation_state(const TessellationState &tessellation_state);

      GraphicsPipelineCreateInfo& set_layout(const PipelineLayout &layout);

      GraphicsPipelineCreateInfo& set_render_pass(const RenderPass &render_pass);

      GraphicsPipelineCreateInfo& set_subpass(uint32_t subpass);

  };  // class GraphicsPipelineCreateInfo

  class GraphicsPipeline
  {
  public:
    GraphicsPipeline(const Device &device, const GraphicsPipelineCreateInfo &create_info);

    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline &) = delete;

    GraphicsPipeline(GraphicsPipeline &&other);

    GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;

    GraphicsPipeline &operator=(GraphicsPipeline &&) = delete;

    VkPipeline get_handle() const;

  private:
    const Device &m_device;

    VkPipeline m_handle;

  }; // class GraphicsPipeline
} // namespace prism