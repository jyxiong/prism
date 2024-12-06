#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/render_pass.h"
#include "prism/vulkan/image_view.h"

namespace prism
{
  class Framebuffer
  {
    public:
      Framebuffer(const Device &device, const RenderPass &render_pass, const ImageView& attachment, uint32_t width, uint32_t height, uint32_t layers = 1);
      
      Framebuffer(const Device &device, const RenderPass &render_pass, const std::vector<ImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers = 1);
      
      Framebuffer(const Device &device, const RenderPass &render_pass, const std::vector<ImageView>& color_attachments, const ImageView& depth_attachment, uint32_t width, uint32_t height, uint32_t layers = 1);
      
      ~Framebuffer();

      Framebuffer(const Framebuffer &) = delete;

      Framebuffer(Framebuffer &&other) noexcept;

      Framebuffer& operator=(const Framebuffer &) = delete;

      Framebuffer& operator=(Framebuffer &&) = delete;

      VkFramebuffer get_handle() const;

    private:
      VkFramebuffer m_handle;

      const Device &m_device;
  }; // class Framebuffer
} // namespace prism