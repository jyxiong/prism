#include "prism/vulkan/render_pass.h"

#include "prism/vulkan/error.h"

using namespace prism;

AttachmentDescription &AttachmentDescription::set_flags(
    VkAttachmentDescriptionFlags flags) {
  this->flags = flags;
  return *this;
}

AttachmentDescription &AttachmentDescription::set_format(VkFormat format) {
  this->format = format;
  return *this;
}

AttachmentDescription &AttachmentDescription::set_samples(
    VkSampleCountFlagBits samples) {
  this->samples = samples;
  return *this;
}

AttachmentDescription &AttachmentDescription::set_load_op(VkAttachmentLoadOp load_op) {
  this->loadOp = load_op;
  return *this;
}

AttachmentDescription &AttachmentDescription::set_store_op(VkAttachmentStoreOp store_op) {
  this->storeOp = store_op;
  return *this;
}

AttachmentDescription &AttachmentDescription::set_stencil_load_op(
    VkAttachmentLoadOp stencil_load_op) {
  this->stencilLoadOp = stencil_load_op;
  return *this;
}

AttachmentDescription &AttachmentDescription::set_stencil_store_op(
    VkAttachmentStoreOp stencil_store_op) {
  this->stencilStoreOp = stencil_store_op;
  return *this;
}

AttachmentDescription &AttachmentDescription::set_initial_layout(
    VkImageLayout initial_layout) {
  this->initialLayout = initial_layout;
  return *this;
}

AttachmentDescription &AttachmentDescription::set_final_layout(
    VkImageLayout final_layout) {
  this->finalLayout = final_layout;
  return *this;
}

AttachmentReference &AttachmentReference::set_attachment(uint32_t attachment) {
  this->attachment = attachment;
  return *this;
}

AttachmentReference &AttachmentReference::set_layout(VkImageLayout layout) {
  this->layout = layout;
  return *this;
}

SubpassDescription &SubpassDescription::set_flags(VkSubpassDescriptionFlags flags) {
  this->flags = flags;
  return *this;
}

SubpassDescription &SubpassDescription::set_pipeline_bind_point(
    VkPipelineBindPoint pipeline_bind_point) {
  this->pipelineBindPoint = pipeline_bind_point;
  return *this;
}

SubpassDescription &SubpassDescription::set_input_attachments(
    const std::vector<AttachmentReference> &attachments) {
  this->inputAttachmentCount = static_cast<uint32_t>(attachments.size());
  this->pInputAttachments = attachments.data();
  return *this;
}

SubpassDescription &SubpassDescription::set_color_attachments(
    const std::vector<AttachmentReference> &attachments) {
  this->colorAttachmentCount = static_cast<uint32_t>(attachments.size());
  this->pColorAttachments = attachments.data();
  return *this;
}

SubpassDescription &SubpassDescription::set_resolve_attachments(
    const std::vector<AttachmentReference> &attachments) {
  this->pResolveAttachments = attachments.data();
  return *this;
}

SubpassDescription &SubpassDescription::set_depth_stencil_attachment(
    const AttachmentReference &attachment) {
  this->pDepthStencilAttachment = &attachment;
  return *this;
}

SubpassDescription &SubpassDescription::set_preserve_attachments(
    const std::vector<uint32_t> &attachments) {
  this->preserveAttachmentCount = static_cast<uint32_t>(attachments.size());
  this->pPreserveAttachments = attachments.data();
  return *this;
}

SubpassDependency &SubpassDependency::set_src_subpass(uint32_t src_subpass) {
  this->srcSubpass = src_subpass;
  return *this;
}

SubpassDependency &SubpassDependency::set_dst_subpass(uint32_t dst_subpass) {
  this->dstSubpass = dst_subpass;
  return *this;
}

SubpassDependency &SubpassDependency::set_src_stage_mask(
    VkPipelineStageFlags src_stage_mask) {
  this->srcStageMask = src_stage_mask;
  return *this;
}

SubpassDependency &SubpassDependency::set_dst_stage_mask(
    VkPipelineStageFlags dst_stage_mask) {
  this->dstStageMask = dst_stage_mask;
  return *this;
}

SubpassDependency &SubpassDependency::set_src_access_mask(
    VkAccessFlags src_access_mask) {
  this->srcAccessMask = src_access_mask;
  return *this;
}

SubpassDependency &SubpassDependency::set_dst_access_mask(
    VkAccessFlags dst_access_mask) {
  this->dstAccessMask = dst_access_mask;
  return *this;
}

SubpassDependency &SubpassDependency::set_dependency_flags(
    VkDependencyFlags dependency_flags) {
  this->dependencyFlags = dependency_flags;
  return *this;
}

RenderPass::RenderPass(const Device &device,
                       const std::vector<AttachmentDescription> &attachments,
                       const std::vector<SubpassDescription> &subpasses,
                       const std::vector<SubpassDependency> &dependencies)
    : m_device(device) {

  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount =
      static_cast<uint32_t>(attachments.size());
  render_pass_info.pAttachments = attachments.data();
  render_pass_info.subpassCount = static_cast<uint32_t>(subpasses.size());
  render_pass_info.pSubpasses = subpasses.data();
  render_pass_info.dependencyCount =
      static_cast<uint32_t>(dependencies.size());
  render_pass_info.pDependencies = dependencies.data();

  VK_CHECK(vkCreateRenderPass(m_device.get_handle(), &render_pass_info, nullptr,
                              &m_handle));
}

RenderPass::~RenderPass() {
  if (m_handle != VK_NULL_HANDLE) {
    vkDestroyRenderPass(m_device.get_handle(), m_handle, nullptr);
  }
}

RenderPass::RenderPass(RenderPass &&other) noexcept
    : m_device(other.m_device),
      m_handle(std::exchange(other.m_handle, nullptr)) {}

VkRenderPass RenderPass::get_handle() const { return m_handle; }