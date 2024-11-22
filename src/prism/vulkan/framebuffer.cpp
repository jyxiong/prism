#include "prism/vulkan/framebuffer.h"

using namespace prism;

Framebuffer::Framebuffer(const Device &device, const RenderPass &render_pass, const ImageView &attachment, uint32_t width, uint32_t height, uint32_t layers)
    : m_device(device) {

  VkFramebufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  create_info.renderPass = render_pass.get_handle();
  create_info.attachmentCount = 1;
  create_info.pAttachments = &attachment.get_handle();
  create_info.width = width;
  create_info.height = height;
  create_info.layers = layers;

  VK_CHECK(vkCreateFramebuffer(m_device.get_handle(), &create_info, nullptr, &m_handle));
}

Framebuffer::Framebuffer(const Device &device, const RenderPass &render_pass, const std::vector<ImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers)
    : m_device(device) {
  std::vector<VkImageView> vk_attachments;
  vk_attachments.reserve(attachments.size());
  for (const auto &attachment : attachments) {
    vk_attachments.push_back(attachment.get_handle());
  }

  VkFramebufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  create_info.renderPass = render_pass.get_handle();
  create_info.attachmentCount = static_cast<uint32_t>(vk_attachments.size());
  create_info.pAttachments = vk_attachments.data();
  create_info.width = width;
  create_info.height = height;
  create_info.layers = layers;
  
  VK_CHECK(vkCreateFramebuffer(m_device.get_handle(), &create_info, nullptr, &m_handle));
}

Framebuffer::Framebuffer(Framebuffer &&other) noexcept
    : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
      m_device(other.m_device) {}

Framebuffer::~Framebuffer() {
  if (m_handle != VK_NULL_HANDLE) {
    vkDestroyFramebuffer(m_device.get_handle(), m_handle, nullptr);
  }
}

VkFramebuffer Framebuffer::get_handle() const { return m_handle; }

