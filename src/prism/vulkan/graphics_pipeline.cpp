#include "prism/vulkan/graphics_pipeline.h"

using namespace prism;

GraphicsPipelineCreateInfo::GraphicsPipelineCreateInfo()
{
  sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  basePipelineIndex = -1;
  basePipelineHandle = VK_NULL_HANDLE;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_shader_stages(const std::vector<ShaderStage> &shader_stages)
{
  stageCount = static_cast<uint32_t>(shader_stages.size());
  pStages = shader_stages.data();
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_vertex_input_state(const VertexInputState &vertex_input_state)
{
  pVertexInputState = &vertex_input_state;
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_input_assembly_state(const InputAssemblyState &input_assembly_state)
{
  pInputAssemblyState = &input_assembly_state;
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_viewport_state(const ViewportState &viewport_state)
{
  pViewportState = &viewport_state;
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_rasterization_state(const RasterizationState &rasterization_state)
{
  pRasterizationState = &rasterization_state;
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_multisample_state(const MultisampleState &multisample_state)
{
  pMultisampleState = &multisample_state;
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_depth_stencil_state(const DepthStencilState &depth_stencil_state)
{
  pDepthStencilState = &depth_stencil_state;
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_color_blend_state(const ColorBlendState &color_blend_state)
{
  pColorBlendState = &color_blend_state;
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_dynamic_state(const DynamicState &dynamic_state)
{
  pDynamicState = &dynamic_state;
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_tesellation_state(const TessellationState &tessellation_state)
{
  pTessellationState = &tessellation_state;
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_layout(const PipelineLayout &_layout)
{
  layout = _layout.get_handle();
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_render_pass(const RenderPass &_render_pass)
{
  renderPass = _render_pass.get_handle();
  return *this;
}

GraphicsPipelineCreateInfo& GraphicsPipelineCreateInfo::set_subpass(uint32_t _subpass)
{
  subpass = _subpass;
  return *this;
}

GraphicsPipeline::GraphicsPipeline(const Device &device, const GraphicsPipelineCreateInfo &create_info)
  : m_device(device)
{
  if (vkCreateGraphicsPipelines(m_device.get_handle(), VK_NULL_HANDLE, 1, reinterpret_cast<const VkGraphicsPipelineCreateInfo*>(&create_info), nullptr, &m_handle) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create graphics pipeline");
  }
}

GraphicsPipeline::~GraphicsPipeline()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(m_device.get_handle(), m_handle, nullptr);
  }
}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline &&other)
  : m_device(other.m_device), m_handle(other.m_handle)
{
  other.m_handle = VK_NULL_HANDLE;
}

VkPipeline GraphicsPipeline::get_handle() const
{
  return m_handle;
}