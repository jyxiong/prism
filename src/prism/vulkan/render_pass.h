#pragma once

#include "prism/vulkan/device.h"

namespace prism {

struct AttachmentDescription : public VkAttachmentDescription {
  AttachmentDescription() = default;

  AttachmentDescription& set_flags(VkAttachmentDescriptionFlags flags);

  AttachmentDescription& set_format(VkFormat format);

  AttachmentDescription& set_samples(VkSampleCountFlagBits samples);

  AttachmentDescription& set_load_op(VkAttachmentLoadOp load_op);

  AttachmentDescription& set_store_op(VkAttachmentStoreOp store_op);

  AttachmentDescription& set_stencil_load_op(VkAttachmentLoadOp stencil_load_op);

  AttachmentDescription& set_stencil_store_op(VkAttachmentStoreOp stencil_store_op);

  AttachmentDescription& set_initial_layout(VkImageLayout initial_layout);

  AttachmentDescription& set_final_layout(VkImageLayout final_layout);
  
};

struct AttachmentReference : public VkAttachmentReference {
  AttachmentReference() = default;

  AttachmentReference& set_attachment(uint32_t attachment);

  AttachmentReference& set_layout(VkImageLayout layout);
};

struct SubpassDescription : public VkSubpassDescription {
  SubpassDescription() = default;

  SubpassDescription& set_flags(VkSubpassDescriptionFlags flags);

  SubpassDescription& set_pipeline_bind_point(VkPipelineBindPoint pipeline_bind_point);

  SubpassDescription& set_input_attachments(const std::vector<AttachmentReference> &attachments);

  SubpassDescription& set_color_attachments(const std::vector<AttachmentReference> &attachments);

  SubpassDescription& set_resolve_attachments(const std::vector<AttachmentReference> &attachments);

  SubpassDescription& set_depth_stencil_attachment(const AttachmentReference &attachment);

  SubpassDescription& set_preserve_attachments(const std::vector<uint32_t> &attachments);
};

struct SubpassDependency : public VkSubpassDependency {
  SubpassDependency() = default;

  SubpassDependency& set_src_subpass(uint32_t src_subpass);

  SubpassDependency& set_dst_subpass(uint32_t dst_subpass);

  SubpassDependency& set_src_stage_mask(VkPipelineStageFlags src_stage_mask);

  SubpassDependency& set_dst_stage_mask(VkPipelineStageFlags dst_stage_mask);

  SubpassDependency& set_src_access_mask(VkAccessFlags src_access_mask);

  SubpassDependency& set_dst_access_mask(VkAccessFlags dst_access_mask);

  SubpassDependency& set_dependency_flags(VkDependencyFlags dependency_flags);

};

class RenderPass {
public:
  RenderPass(const Device &device,
             const std::vector<AttachmentDescription> &attachments,
             const std::vector<SubpassDescription> &subpasses,
             const std::vector<SubpassDependency> &dependencies);

  ~RenderPass();

  RenderPass(const RenderPass &) = delete;

  RenderPass(RenderPass &&other) noexcept;

  RenderPass &operator=(const RenderPass &) = delete;

  RenderPass &operator=(RenderPass &&other) = delete;

  VkRenderPass get_handle() const;

private:
  const Device &m_device;
  VkRenderPass m_handle;
}; // class RenderPass

} // namespace prism