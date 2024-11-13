#pragma once

namespace prism {
class VertexInputState : public VkPipelineVertexInputStateCreateInfo {
public:
  VertexInputState();
  VertexInputState &set_binding_descriptions(
      const std::vector<VkVertexInputBindingDescription> &bindings);
  VertexInputState &set_attribute_descriptions(
      const std::vector<VkVertexInputAttributeDescription> &attributes);
};

class InputAssemblyState : public VkPipelineInputAssemblyStateCreateInfo {
public:
  InputAssemblyState();
  InputAssemblyState &set_topology(VkPrimitiveTopology topology);
  InputAssemblyState &
  set_primitive_restart_enable(VkBool32 primitive_restart_enable);
};

class ViewportState : public VkPipelineViewportStateCreateInfo {

public:
  ViewportState();
  ViewportState &set_viewports(const std::vector<VkViewport> &viewports);
  ViewportState &set_scissors(const std::vector<VkRect2D> &scissors);
};

class RasterizationState : public VkPipelineRasterizationStateCreateInfo {
public:
  RasterizationState();
  RasterizationState &set_depth_clamp_enable(VkBool32 depth_clamp_enable);
  RasterizationState &
  set_rasterizer_discard_enable(VkBool32 rasterizer_discard_enable);
  RasterizationState &set_polygon_mode(VkPolygonMode polygon_mode);
  RasterizationState &set_cull_mode(VkCullModeFlags cull_mode);
  RasterizationState &set_front_face(VkFrontFace front_face);
  RasterizationState &set_depth_bias_enable(VkBool32 depth_bias_enable);
  RasterizationState &
  set_depth_bias_constant_factor(float depth_bias_constant_factor);
  RasterizationState &set_depth_bias_clamp(float depth_bias_clamp);
  RasterizationState &
  set_depth_bias_slope_factor(float depth_bias_slope_factor);
  RasterizationState &set_line_width(float line_width);
};

class MultisampleState : public VkPipelineMultisampleStateCreateInfo {
public:
  MultisampleState();
  MultisampleState &
  set_rasterization_samples(VkSampleCountFlagBits rasterization_samples);
  MultisampleState &set_sample_shading_enable(VkBool32 sample_shading_enable);
  MultisampleState &set_min_sample_shading(float min_sample_shading);
  MultisampleState &set_sample_mask(uint32_t sample_mask);
  MultisampleState &
  set_alpha_to_coverage_enable(VkBool32 alpha_to_coverage_enable);
  MultisampleState &set_alpha_to_one_enable(VkBool32 alpha_to_one_enable);
};

class DepthStencilState : public VkPipelineDepthStencilStateCreateInfo {
public:
  DepthStencilState();
  DepthStencilState &set_depth_test_enable(VkBool32 depth_test_enable);
  DepthStencilState &set_depth_write_enable(VkBool32 depth_write_enable);
  DepthStencilState &set_depth_compare_op(VkCompareOp depth_compare_op);
  DepthStencilState &
  set_depth_bounds_test_enable(VkBool32 depth_bounds_test_enable);
  DepthStencilState &set_stencil_test_enable(VkBool32 stencil_test_enable);
  DepthStencilState &set_front(VkStencilOpState front);
  DepthStencilState &set_back(VkStencilOpState back);
};

class ColorBlendState : public VkPipelineColorBlendStateCreateInfo {
public:
  ColorBlendState();
  ColorBlendState &set_logic_op_enable(VkBool32 logic_op_enable);
  ColorBlendState &set_logic_op(VkLogicOp logic_op);
  ColorBlendState &set_attachments(
      const std::vector<VkPipelineColorBlendAttachmentState> &attachments);
};

class DynamicState : public VkPipelineDynamicStateCreateInfo {
public:
  DynamicState();
  DynamicState &
  set_dynamic_states(const std::vector<VkDynamicState> &dynamic_states);
};

class TessellationState : public VkPipelineTessellationStateCreateInfo {
public:
  TessellationState();
  TessellationState &set_patch_control_points(uint32_t patch_control_points);
};

} // namespace prism