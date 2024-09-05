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

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface.get_handle();
  create_info.imageFormat = m_properties.surface_format.format;
  create_info.imageColorSpace = m_properties.surface_format.colorSpace;
  create_info.presentMode = m_properties.present_mode;
  create_info.imageExtent = m_properties.extent;
  create_info.minImageCount = m_properties.image_count;
  create_info.imageUsage = m_properties.image_usage;

  create_info.oldSwapchain = old_swapchain.has_value() ? old_swapchain->get_handle() : VK_NULL_HANDLE;
  create_info.preTransform = support_details.capabilities.currentTransform;
  create_info.imageArrayLayers = 1;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  create_info.clipped = VK_TRUE;

  VK_CHECK(vkCreateSwapchainKHR(device.get_handle(), &create_info, nullptr, &m_handle));

  unsigned int image_count;
  VK_CHECK(vkGetSwapchainImagesKHR(m_device.get_handle(), m_handle, &image_count, nullptr));
  m_images.resize(image_count);
  VK_CHECK(vkGetSwapchainImagesKHR(m_device.get_handle(), m_handle, &image_count, m_images.data()));

  VkImageViewCreateInfo image_view_create_info{};
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  image_view_create_info.format = m_properties.surface_format.format;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.layerCount = 1;
  image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  
  for (auto &image : m_images)
  {
    image_view_create_info.image = image;

    VkImageView image_view;
    VK_CHECK(vkCreateImageView(m_device.get_handle(), &image_view_create_info, nullptr, &image_view));
    m_image_views.push_back(image_view);
  }
}

Swapchain::~Swapchain()
{
  if (m_handle != VK_NULL_HANDLE)
  {
    vkDestroySwapchainKHR(m_device.get_handle(), m_handle, nullptr);
  }

  for (auto& image_view : m_image_views)
  {
    if (image_view != VK_NULL_HANDLE)
    {
      vkDestroyImageView(m_device.get_handle(), image_view, nullptr);
    }
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

const std::vector<VkImage> &Swapchain::get_images() const
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
