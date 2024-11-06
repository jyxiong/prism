#include "prism/vulkan/swapchain.h"

#include "prism/vulkan/device.h"
#include "prism/vulkan/surface.h"
#include "prism/vulkan/error.h"

using namespace prism;

struct SwapchainSupportDetails
{
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> surface_formats;
  std::vector<VkPresentModeKHR> present_modes;
};

SwapchainSupportDetails query_swapchain_support(const Device &device, const Surface &surface);

VkSurfaceFormatKHR choose_surface_format(VkSurfaceFormatKHR required, const std::vector<VkSurfaceFormatKHR> &available);

VkPresentModeKHR choose_present_mode(const VkPresentModeKHR &required, const std::vector<VkPresentModeKHR> &available);

VkExtent2D choose_extent(VkExtent2D required, const VkSurfaceCapabilitiesKHR &capabilities);

uint32_t choose_image_count(uint32_t required, const VkSurfaceCapabilitiesKHR &capabilities);

SwapchainImage::SwapchainImage(const Device &device, const ImageCreateInfo& info, VkImage handle)
    : Image(device, info, handle)
{
}

SwapchainImage::SwapchainImage(SwapchainImage &&other) noexcept
    : Image(std::move(other))
{
}

SwapchainImage::~SwapchainImage()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    m_handle = VK_NULL_HANDLE;
  }
}

SwapchainCreateInfo::SwapchainCreateInfo()
{

}

Swapchain::Swapchain(const Device &device, const Surface &surface, const Properties &required, const std::optional<Swapchain> &old_swapchain)
    : m_device(device), m_surface(surface), m_properties(required)
{
  auto support_details = query_swapchain_support(device, surface);

  m_properties.surface_format = choose_surface_format(required.surface_format, support_details.surface_formats);

  m_properties.present_mode = choose_present_mode(required.present_mode, support_details.present_modes);

  m_properties.extent = choose_extent(required.extent, support_details.capabilities);

  m_properties.image_count = choose_image_count(required.image_count, support_details.capabilities);

  VkSwapchainCreateInfoKHR ci{};
  ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  ci.surface = surface.get_handle();
  ci.imageFormat = m_properties.surface_format.format;
  ci.imageColorSpace = m_properties.surface_format.colorSpace;
  ci.presentMode = m_properties.present_mode;
  ci.imageExtent = m_properties.extent;
  ci.minImageCount = m_properties.image_count;
  ci.imageUsage = m_properties.image_usage;

  ci.oldSwapchain = old_swapchain.has_value() ? old_swapchain->get_handle() : VK_NULL_HANDLE;
  ci.preTransform = support_details.capabilities.currentTransform;
  ci.imageArrayLayers = 1;
  ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  ci.clipped = VK_TRUE;

  VK_CHECK(vkCreateSwapchainKHR(device.get_handle(), &ci, nullptr, &m_handle));

  unsigned int image_count;
  VK_CHECK(vkGetSwapchainImagesKHR(m_device.get_handle(), m_handle, &image_count, nullptr));
  std::vector<VkImage> vk_images(image_count);
  VK_CHECK(vkGetSwapchainImagesKHR(m_device.get_handle(), m_handle, &image_count, vk_images.data()));

  ImageCreateInfo image_ci{};
  image_ci.set_image_type(VK_IMAGE_TYPE_2D)
          .set_format(m_properties.surface_format.format)
          .set_extent({m_properties.extent.width, m_properties.extent.height, 1})
          .set_usage(m_properties.image_usage);

  ImageViewCreateInfo image_view_ci{};
  image_view_ci.set_view_type(VK_IMAGE_VIEW_TYPE_2D)
      .set_format(m_properties.surface_format.format)
      .set_level_count(1)
      .set_layer_count(1)
      .set_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);

  for (auto &vk_image : vk_images)
  {
    m_images.emplace_back(m_device, image_ci, vk_image);

    image_view_ci.image = vk_image;
    m_image_views.emplace_back(m_images.back(), image_view_ci);
  }
}

Swapchain::Swapchain(Swapchain &&other) noexcept
    : m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
      m_device(other.m_device),
      m_surface(other.m_surface),
      m_images(std::move(other.m_images)),
      m_image_views(std::move(other.m_image_views)),
      m_properties(other.m_properties)
{
}

Swapchain::~Swapchain()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroySwapchainKHR(m_device.get_handle(), m_handle, nullptr);
  }
}

VkSwapchainKHR Swapchain::get_handle() const
{
  return m_handle;
}

const VkFormat &Swapchain::get_format() const
{
  return m_properties.surface_format.format;
}

const VkExtent2D &Swapchain::get_extent() const
{
  return m_properties.extent;
}

const std::vector<SwapchainImage> &Swapchain::get_images() const
{
  return m_images;
}

VkResult Swapchain::acquire_next_image(uint64_t time_out, const Semaphore& semaphore, const std::optional<Fence>& fence, uint32_t &image_index) const
{
  auto fence_handle = fence.has_value() ? fence->get_handle() : VK_NULL_HANDLE;
  return vkAcquireNextImageKHR(m_device.get_handle(), m_handle, time_out, semaphore.get_handle(), fence_handle, &image_index);
}

SwapchainSupportDetails query_swapchain_support(const Device &device, const Surface &surface)
{
  SwapchainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.get_physical_device().get_handle(), surface.get_handle(), &details.capabilities);

  unsigned int format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device.get_physical_device().get_handle(), surface.get_handle(), &format_count, nullptr);
  details.surface_formats.resize(format_count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(device.get_physical_device().get_handle(), surface.get_handle(), &format_count, details.surface_formats.data());

  unsigned int present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device.get_physical_device().get_handle(), surface.get_handle(), &present_mode_count, nullptr);
  details.present_modes.resize(present_mode_count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(device.get_physical_device().get_handle(), surface.get_handle(), &present_mode_count, details.present_modes.data());

  return details;
}

VkSurfaceFormatKHR choose_surface_format(VkSurfaceFormatKHR required, const std::vector<VkSurfaceFormatKHR> &available)
{
  auto find_result = std::find_if(available.begin(), available.end(), [&](const VkSurfaceFormatKHR &available)
                                  { return available.format == required.format && available.colorSpace == required.colorSpace; });

  if (find_result != available.end())
  {
    return required;
  }
  else
  {
    // TODO
    return available[0];
  }
}

VkPresentModeKHR choose_present_mode(const VkPresentModeKHR &required, const std::vector<VkPresentModeKHR> &available)
{
  auto find_result = std::find(available.begin(), available.end(), required);
  if (find_result != available.end())
  {
    return required;
  }
  else
  {
    // TODO
    return VK_PRESENT_MODE_FIFO_KHR;
  }
}

VkExtent2D choose_extent(VkExtent2D required, const VkSurfaceCapabilitiesKHR &capabilities)
{
  if (capabilities.currentExtent.width == UINT32_MAX)
  {
    return required;
  }

  if (required.width < 1 || required.height < 1)
  {
    return capabilities.currentExtent;
  }

  VkExtent2D actual_extent = required;
  actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
  actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));
  return actual_extent;
}

uint32_t choose_image_count(uint32_t required, const VkSurfaceCapabilitiesKHR &capabilities)
{
  return std::min(std::max(required, capabilities.minImageCount), capabilities.maxImageCount);
}
