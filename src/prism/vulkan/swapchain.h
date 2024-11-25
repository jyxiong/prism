#pragma once

#include "prism/vulkan/semaphore.h"
#include "prism/vulkan/fence.h"
#include "prism/vulkan/image.h"
#include "prism/vulkan/image_view.h"

namespace prism
{
  class Device;
  class Surface;

  class SwapchainImage : public Image
  {
    public:
      SwapchainImage(const Device &device, const ImageCreateInfo& info, VkImage handle);

      SwapchainImage(const SwapchainImage &) = delete;

      SwapchainImage(SwapchainImage &&other) noexcept;

      ~SwapchainImage() override;

      SwapchainImage &operator=(const SwapchainImage &) = delete;

      SwapchainImage &operator=(SwapchainImage &&) = delete;
  };

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

    Swapchain(const Swapchain &) = delete;

    Swapchain(Swapchain &&other) noexcept;

    ~Swapchain();

    Swapchain &operator=(const Swapchain &) = delete;

    Swapchain &operator=(Swapchain &&) = delete;

    VkSwapchainKHR get_handle() const;

    const VkFormat &get_format() const;

    const VkExtent2D &get_extent() const;

    const std::vector<SwapchainImage> &get_images() const;

    VkResult acquire_next_image(uint64_t time_out, const Semaphore& semaphore, const Fence& fence, uint32_t &image_index);

  private:
    VkSwapchainKHR m_handle{};

    const Device &m_device;

    const Surface &m_surface;

    std::vector<SwapchainImage> m_images;

    Properties m_properties;

  }; // class SwapChain

} // namespace prism
