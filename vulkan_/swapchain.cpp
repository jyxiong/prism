#include "comet/vulkan/swapchain.h"

#include <algorithm>
#include <stdexcept>

#include "comet/vulkan/device.h"

using namespace comet;

inline VkSurfaceFormatKHR choose_surface_format(VkSurfaceFormatKHR required, const std::vector<VkSurfaceFormatKHR> &available);

inline VkPresentModeKHR choose_present_mode(const VkPresentModeKHR &required, const std::vector<VkPresentModeKHR> &available);

inline VkExtent2D choose_extent(VkExtent2D required, const VkSurfaceCapabilitiesKHR &capabilities);

inline uint32_t choose_image_count(uint32_t required, const VkSurfaceCapabilitiesKHR &capabilities);

inline uint32_t choose_array_layers(uint32_t required, const VkSurfaceCapabilitiesKHR &capabilities);

inline bool validate_format_feature(VkImageUsageFlagBits image_usage, VkFormatFeatureFlags supported_features);
inline std::set<VkImageUsageFlagBits> choose_image_usage_flags(const std::set<VkImageUsageFlagBits> &required, const VkSurfaceCapabilitiesKHR &capabilities, const VkFormatProperties &format_properties);
inline VkImageUsageFlags composite_image_usage_flags(std::set<VkImageUsageFlagBits> &image_usage);

inline VkSurfaceTransformFlagBitsKHR choose_transform(VkSurfaceTransformFlagBitsKHR required, const VkSurfaceCapabilitiesKHR &capabilities);

inline VkCompositeAlphaFlagBitsKHR choose_composite_alpha(VkCompositeAlphaFlagBitsKHR required, const VkSurfaceCapabilitiesKHR &capabilities);

Swapchain::Swapchain(Device &device, const VkSurfaceKHR &surface,
                     const VkPresentModeKHR &present_mode,
                     const std::vector<VkPresentModeKHR> &present_mode_priority_list,
                     const VkSurfaceFormatKHR &surface_format,
                     const std::vector<VkSurfaceFormatKHR> &surface_format_priority_list,
                     const VkExtent2D &extent,
                     uint32_t image_count,
                     uint32_t array_layers,
                     const VkSurfaceTransformFlagBitsKHR transform,
                     const std::set<VkImageUsageFlagBits> &image_usage_flags)
    : Swapchain(*this, device, surface, present_mode, present_mode_priority_list, surface_format, surface_format_priority_list, extent, image_count, array_layers, transform, image_usage_flags)
{
}

Swapchain::Swapchain(Swapchain &old_swapchain, Device &device, const VkSurfaceKHR &surface,
                     const VkPresentModeKHR &present_mode,
                     const std::vector<VkPresentModeKHR> &present_mode_priority_list,
                     const VkSurfaceFormatKHR &surface_format,
                     const std::vector<VkSurfaceFormatKHR> &surface_format_priority_list,
                     const VkExtent2D &extent,
                     uint32_t image_count,
                     uint32_t array_layers,
                     const VkSurfaceTransformFlagBitsKHR transform,
                     const std::set<VkImageUsageFlagBits> &image_usage_flags)
    : m_device(device), m_surface(surface), m_image_usage_flags(image_usage_flags)
{
    // 获取物理设备表面格式
    unsigned int surface_format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.get_physical_device().get_handle(), m_surface, &surface_format_count, nullptr);
    m_surface_formats.resize(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.get_physical_device().get_handle(), m_surface, &surface_format_count, m_surface_formats.data());

    // 获取物理设备表面呈现模式
    unsigned int present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_device.get_physical_device().get_handle(), m_surface, &present_mode_count, nullptr);
    m_present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_device.get_physical_device().get_handle(), m_surface, &present_mode_count, m_present_modes.data());

    // 获取物理设备表面分辨率
    VkSurfaceCapabilitiesKHR surface_capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device.get_physical_device().get_handle(), m_surface, &surface_capabilities);

    // 选择表面格式
    m_properties.surface_format = choose_surface_format(surface_format, m_surface_formats);

    // 选择呈现模式
    m_properties.present_mode = choose_present_mode(present_mode, m_present_modes);

    // 选择分辨率
    m_properties.extent = choose_extent({800, 600}, surface_capabilities);

    // 选择图像数量
    m_properties.image_count = choose_image_count(image_count, surface_capabilities);

    // 选择数组层数
    m_properties.array_layers = choose_array_layers(array_layers, surface_capabilities);

    // 选择图像用途
    m_image_usage_flags = choose_image_usage_flags(image_usage_flags, surface_capabilities, m_device.get_physical_device().get_format_properties(m_properties.surface_format.format));
    m_properties.image_usage = composite_image_usage_flags(m_image_usage_flags);

    // 选择变换
    m_properties.pre_transform = choose_transform(transform, surface_capabilities);

    // 选择合成alpha
    m_properties.composite_alpha = choose_composite_alpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, surface_capabilities);

    // 旧交换链
    m_properties.old_swapchain = old_swapchain.get_handle();

    // 创建交换链
    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = m_surface;
    create_info.minImageCount = m_properties.image_count;
    create_info.imageFormat = m_properties.surface_format.format;
    create_info.imageColorSpace = m_properties.surface_format.colorSpace;
    create_info.imageExtent = m_properties.extent;
    create_info.imageArrayLayers = m_properties.array_layers;
    create_info.imageUsage = m_properties.image_usage;
    create_info.preTransform = m_properties.pre_transform;
    create_info.compositeAlpha = m_properties.composite_alpha;
    create_info.presentMode = m_properties.present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = m_properties.old_swapchain;

    if (vkCreateSwapchainKHR(m_device.get_handle(), &create_info, nullptr, &m_handle) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    // 获取交换链中的图像
    unsigned int swapchain_image_count;
    vkGetSwapchainImagesKHR(m_device.get_handle(), m_handle, &swapchain_image_count, nullptr);
    m_images.resize(swapchain_image_count);
    vkGetSwapchainImagesKHR(m_device.get_handle(), m_handle, &swapchain_image_count, m_images.data());
}

Swapchain::~Swapchain()
{
    vkDestroySwapchainKHR(m_device.get_handle(), m_handle, nullptr);
}

VkSwapchainKHR Swapchain::get_handle() const
{
    return m_handle;
}

const std::vector<VkImage> &Swapchain::get_images() const
{
    return m_images;
}

VkFormat Swapchain::get_format() const
{
    return m_properties.surface_format.format;
}

const VkExtent2D &Swapchain::get_extent() const
{
    return m_properties.extent;
}

VkImageUsageFlags Swapchain::get_usage() const
{
    return m_properties.image_usage;
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
        // TODO: 提供一个可配置的默认值
        // 默认返回第一个可用的格式
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
        // TODO: 提供一个可配置的默认值
        // 默认返回FIFO模式
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

uint32_t choose_array_layers(uint32_t required, const VkSurfaceCapabilitiesKHR &capabilities)
{
    return std::min(std::max(required, 1u), capabilities.maxImageArrayLayers);
}

bool validate_format_feature(VkImageUsageFlagBits image_usage, VkFormatFeatureFlags supported_features)
{
    switch (image_usage)
    {
    case VK_IMAGE_USAGE_STORAGE_BIT:
        return VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT & supported_features;
    default:
        return true;
    }
}

std::set<VkImageUsageFlagBits> choose_image_usage_flags(const std::set<VkImageUsageFlagBits> &required, const VkSurfaceCapabilitiesKHR &capabilities, const VkFormatProperties &format_properties)
{
    std::set<VkImageUsageFlagBits> validated_image_usage_flags;
    for (auto flag : required)
    {
        if ((capabilities.supportedUsageFlags & flag) && (validate_format_feature(flag, format_properties.optimalTilingFeatures)))
        {
            validated_image_usage_flags.insert(flag);
        }
        else
        {
            throw std::runtime_error("failed to find suitable image usage flags!");
        }
    }

    if (validated_image_usage_flags.empty())
    {
        // Pick the first format from list of defaults, if supported
        static const std::vector<VkImageUsageFlagBits> image_usage_flags = {
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_IMAGE_USAGE_STORAGE_BIT,
            VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT};

        for (VkImageUsageFlagBits image_usage : image_usage_flags)
        {
            if ((image_usage & capabilities.supportedUsageFlags) && validate_format_feature(image_usage, format_properties.optimalTilingFeatures))
            {
                validated_image_usage_flags.insert(image_usage);
                break;
            }
        }
    }

    if (validated_image_usage_flags.empty())
    {
        throw std::runtime_error("failed to find suitable image usage flags!");
    }
    else
    {
        
    }

    return validated_image_usage_flags;
}

VkImageUsageFlags composite_image_usage_flags(std::set<VkImageUsageFlagBits> &image_usage)
{
    VkImageUsageFlags result = 0;
    for (auto &flag : image_usage)
    {
        result |= flag;
    }
    return result;
}

VkSurfaceTransformFlagBitsKHR choose_transform(VkSurfaceTransformFlagBitsKHR required, const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.supportedTransforms & required)
    {
        return required;
    }
    else
    {
        return capabilities.currentTransform;
    }
}

VkCompositeAlphaFlagBitsKHR choose_composite_alpha(VkCompositeAlphaFlagBitsKHR required, const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.supportedCompositeAlpha & required)
    {
        return required;
    }
    else
    {
        return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    }
}
