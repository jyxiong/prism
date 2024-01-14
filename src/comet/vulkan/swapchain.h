#pragma once

#include <vector>
#include <set>

#include "vulkan/vulkan.h"

namespace comet
{
	class Device;

	enum ImageFormat
	{
		sRGB,
		UNORM
	};

	struct SwapchainProperties
	{
		VkSwapchainKHR old_swapchain;

		uint32_t image_count{3};
		VkExtent2D extent{};
		uint32_t array_layers;
		VkImageUsageFlags image_usage;
		VkSurfaceTransformFlagBitsKHR pre_transform;
		VkCompositeAlphaFlagBitsKHR composite_alpha;

		VkSurfaceFormatKHR surface_format{};

		VkPresentModeKHR present_mode;
	};

	class Swapchain
	{
	public:
		Swapchain(Device &device, const VkSurfaceKHR &surface,
				  const VkPresentModeKHR &present_mode = VK_PRESENT_MODE_FIFO_KHR,
				  const std::vector<VkPresentModeKHR> &present_mode_priority_list = {VK_PRESENT_MODE_FIFO_KHR,
																					 VK_PRESENT_MODE_MAILBOX_KHR},
				  const VkSurfaceFormatKHR &surface_format = {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
				  const std::vector<VkSurfaceFormatKHR> &surface_format_priority_list = {{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
																						 {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}},
				  const VkExtent2D &extent = {},
				  uint32_t image_count = 3,
				  uint32_t array_layers = 1,
				  const VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
				  const std::set<VkImageUsageFlagBits> &image_usage_flags = {VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT});

		Swapchain(Swapchain &old_swapchain, Device &device, const VkSurfaceKHR &surface,
				  const VkPresentModeKHR &present_mode = VK_PRESENT_MODE_FIFO_KHR,
				  const std::vector<VkPresentModeKHR> &present_mode_priority_list = {VK_PRESENT_MODE_FIFO_KHR,
																					 VK_PRESENT_MODE_MAILBOX_KHR},
				  const VkSurfaceFormatKHR &surface_format = {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
				  const std::vector<VkSurfaceFormatKHR> &surface_format_priority_list = {{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
																						 {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}},
				  const VkExtent2D &extent = {},
				  uint32_t image_count = 3,
				  uint32_t array_layers = 1,
				  const VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
				  const std::set<VkImageUsageFlagBits> &image_usage_flags = {VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT});

		~Swapchain();

		VkSwapchainKHR get_handle() const { return m_handle; }

	private:
		Device &m_device;

		VkSurfaceKHR m_surface{};

		VkSwapchainKHR m_handle{};

		std::vector<VkImage> m_images{};

		std::vector<VkSurfaceFormatKHR> m_surface_formats{};

		std::vector<VkPresentModeKHR> m_present_modes{};

		SwapchainProperties m_properties{};

		std::set<VkImageUsageFlagBits> m_image_usage_flags{};

	}; // class SwapChain

} // namespace comet