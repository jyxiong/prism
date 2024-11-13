#include "prism/vulkan/graphics_pipeline_state.h"

using namespace prism;

VertexInputState::VertexInputState()
{
  sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  vertexBindingDescriptionCount = 0;
  pVertexBindingDescriptions = nullptr;
  vertexAttributeDescriptionCount = 0;
  pVertexAttributeDescriptions = nullptr;
}

VertexInputState &VertexInputState::set_binding_descriptions(const std::vector<VkVertexInputBindingDescription> &bindings)
{
  vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
  pVertexBindingDescriptions = bindings.data();

  return *this;
}

VertexInputState &VertexInputState::set_attribute_descriptions(const std::vector<VkVertexInputAttributeDescription> &attributes)
{
  vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
  pVertexAttributeDescriptions = attributes.data();

  return *this;
}

InputAssemblyState::InputAssemblyState()
{
  sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  primitiveRestartEnable = VK_FALSE;
}

InputAssemblyState &InputAssemblyState::set_topology(VkPrimitiveTopology topology)
{
  this->topology = topology;
  return *this;
}

InputAssemblyState &InputAssemblyState::set_primitive_restart_enable(VkBool32 primitive_restart_enable)
{
  this->primitiveRestartEnable = primitive_restart_enable;
  return *this;
}

ViewportState::ViewportState()
{
  sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  viewportCount = 1;
  pViewports = nullptr;
  scissorCount = 1;
  pScissors = nullptr;
}

ViewportState &ViewportState::set_viewports(const std::vector<VkViewport> &viewports)
{
  viewportCount = static_cast<uint32_t>(viewports.size());
  pViewports = viewports.data();

  return *this;
}

ViewportState &ViewportState::set_scissors(const std::vector<VkRect2D> &scissors)
{
  scissorCount = static_cast<uint32_t>(scissors.size());
  pScissors = scissors.data();

  return *this;
}

RasterizationState::RasterizationState()
{
  sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  depthClampEnable = VK_FALSE;
  rasterizerDiscardEnable = VK_FALSE;
  polygonMode = VK_POLYGON_MODE_FILL;
  cullMode = VK_CULL_MODE_BACK_BIT;
  frontFace = VK_FRONT_FACE_CLOCKWISE;
  depthBiasEnable = VK_FALSE;
  depthBiasConstantFactor = 0.0f;
  depthBiasClamp = 0.0f;
  depthBiasSlopeFactor = 0.0f;
  lineWidth = 1.0f;
}

RasterizationState &RasterizationState::set_depth_clamp_enable(VkBool32 depth_clamp_enable)
{
  this->depthClampEnable = depth_clamp_enable;
  return *this;
}

RasterizationState &RasterizationState::set_rasterizer_discard_enable(VkBool32 rasterizer_discard_enable)
{
  this->rasterizerDiscardEnable = rasterizer_discard_enable;
  return *this;
}

RasterizationState &RasterizationState::set_polygon_mode(VkPolygonMode polygon_mode)
{
  this->polygonMode = polygon_mode;
  return *this;
}

RasterizationState &RasterizationState::set_cull_mode(VkCullModeFlags cull_mode)
{
  this->cullMode = cull_mode;
  return *this;
}

RasterizationState &RasterizationState::set_front_face(VkFrontFace front_face)
{
  this->frontFace = front_face;
  return *this;
}

RasterizationState &RasterizationState::set_depth_bias_enable(VkBool32 depth_bias_enable)
{
  this->depthBiasEnable = depth_bias_enable;
  return *this;
}

RasterizationState &RasterizationState::set_depth_bias_constant_factor(float depth_bias_constant_factor)
{
  this->depthBiasConstantFactor = depth_bias_constant_factor;
  return *this;
}

RasterizationState &RasterizationState::set_depth_bias_clamp(float depth_bias_clamp)
{
  this->depthBiasClamp = depth_bias_clamp;
  return *this;
}

RasterizationState &RasterizationState::set_depth_bias_slope_factor(float depth_bias_slope_factor)
{
  this->depthBiasSlopeFactor = depth_bias_slope_factor;
  return *this;
}

RasterizationState &RasterizationState::set_line_width(float line_width)
{
  this->lineWidth = line_width;
  return *this;
}

MultisampleState::MultisampleState()
{
  sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  sampleShadingEnable = VK_FALSE;
  minSampleShading = 1.0f;
  pSampleMask = nullptr;
  alphaToCoverageEnable = VK_FALSE;
  alphaToOneEnable = VK_FALSE;
}

MultisampleState &MultisampleState::set_rasterization_samples(VkSampleCountFlagBits rasterization_samples)
{
  this->rasterizationSamples = rasterization_samples;
  return *this;
}

MultisampleState &MultisampleState::set_sample_shading_enable(VkBool32 sample_shading_enable)
{
  this->sampleShadingEnable = sample_shading_enable;
  return *this;
}

MultisampleState &MultisampleState::set_min_sample_shading(float min_sample_shading)
{
  this->minSampleShading = min_sample_shading;
  return *this;
}

MultisampleState &MultisampleState::set_sample_mask(uint32_t sample_mask)
{
  this->pSampleMask = &sample_mask;
  return *this;
}

MultisampleState &MultisampleState::set_alpha_to_coverage_enable(VkBool32 alpha_to_coverage_enable)
{
  this->alphaToCoverageEnable = alpha_to_coverage_enable;
  return *this;
}

MultisampleState &MultisampleState::set_alpha_to_one_enable(VkBool32 alpha_to_one_enable)
{
  this->alphaToOneEnable = alpha_to_one_enable;
  return *this;
}

DepthStencilState::DepthStencilState()
{
  sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  depthTestEnable = VK_TRUE;
  depthWriteEnable = VK_TRUE;
  depthCompareOp = VK_COMPARE_OP_LESS;
  depthBoundsTestEnable = VK_FALSE;
  stencilTestEnable = VK_FALSE;
  front = {};
  back = {};
}

DepthStencilState &DepthStencilState::set_depth_test_enable(VkBool32 depth_test_enable)
{
  this->depthTestEnable = depth_test_enable;
  return *this;
}

DepthStencilState &DepthStencilState::set_depth_write_enable(VkBool32 depth_write_enable)
{
  this->depthWriteEnable = depth_write_enable;
  return *this;
}

DepthStencilState &DepthStencilState::set_depth_compare_op(VkCompareOp depth_compare_op)
{
  this->depthCompareOp = depth_compare_op;
  return *this;
}

DepthStencilState &DepthStencilState::set_depth_bounds_test_enable(VkBool32 depth_bounds_test_enable)
{
  this->depthBoundsTestEnable = depth_bounds_test_enable;
  return *this;
}

DepthStencilState &DepthStencilState::set_stencil_test_enable(VkBool32 stencil_test_enable)
{
  this->stencilTestEnable = stencil_test_enable;
  return *this;
}

DepthStencilState &DepthStencilState::set_front(VkStencilOpState front)
{
  this->front = front;
  return *this;
}

DepthStencilState &DepthStencilState::set_back(VkStencilOpState back)
{
  this->back = back;
  return *this;
}

ColorBlendState::ColorBlendState()
{
  sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  logicOpEnable = VK_FALSE;
  logicOp = VK_LOGIC_OP_COPY;
  attachmentCount = 0;
  pAttachments = nullptr;
  blendConstants[0] = 0.0f;
  blendConstants[1] = 0.0f;
  blendConstants[2] = 0.0f;
  blendConstants[3] = 0.0f;
}

ColorBlendState &ColorBlendState::set_logic_op_enable(VkBool32 logic_op_enable)
{
  this->logicOpEnable = logic_op_enable;
  return *this;
}

ColorBlendState &ColorBlendState::set_logic_op(VkLogicOp logic_op)
{
  this->logicOp = logic_op;
  return *this;
}

ColorBlendState &ColorBlendState::set_attachments(const std::vector<VkPipelineColorBlendAttachmentState> &attachments)
{
  attachmentCount = static_cast<uint32_t>(attachments.size());
  pAttachments = attachments.data();

  return *this;
}

DynamicState::DynamicState()
{
  sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  dynamicStateCount = 0;
  pDynamicStates = nullptr;
}

DynamicState &DynamicState::set_dynamic_states(const std::vector<VkDynamicState> &dynamic_states)
{
  dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
  pDynamicStates = dynamic_states.data();

  return *this;
}

TessellationState::TessellationState()
{
  sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
  pNext = nullptr;
  flags = 0;
  patchControlPoints = 0;
}

TessellationState &TessellationState::set_patch_control_points(uint32_t patch_control_points)
{
  this->patchControlPoints = patch_control_points;
  return *this;
}