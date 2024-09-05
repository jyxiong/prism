#pragma once

#include "prism/vulkan/semaphore.h"
#include "prism/vulkan/fence.h"

namespace prism
{
  class Device;
  class Surface;

  class SwapchainCreateInfo : public VkSwapchainCreateInfoKHR
  {
  public:
    SwapchainCreateInfo();
  };

  class Swapchain
  {
  public:
    struct Properties
    {
      VkExtent2D extent{};
      uint32_t image_count{3};
      VkPresentModeKHR present_mode{VK_PRESENT_MODE_FIFO_KHR};
      VkSurfaceFormatKHR surface_format{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
      VkImageUsageFlags image_usage{VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT};
    };

  public:
    Swapchain(const Device &device, const Surface &surface, const Properties &required, const std::optional<Swapchain> &old_swapchain = std::nullopt);

    ~Swapchain();

    VkSwapchainKHR get_handle() const;

    const VkFormat &get_format() const;

    const VkExtent2D &get_extent() const;

    const std::vector<VkImage> &get_images() const;

    VkResult acquire_next_image(uint64_t time_out, const Semaphore& semaphore, const std::optional<Fence>& fence, uint32_t &image_index) const;

  private:
    VkSwapchainKHR m_handle{};

    const Device &m_device;

    const Surface &m_surface;

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_image_views;

    Properties m_properties;

  }; // class SwapChain

} // namespace prism
