#include "prism/vulkan/image.h"

#include "prism/vulkan/error.h"
#include "prism/vulkan/buffer.h"
#include "prism/vulkan/device_memory.h"
#include "prism/vulkan/utils.h"
#include "prism/vulkan/command_buffer.h"

using namespace prism;

ImageCreateInfo::ImageCreateInfo()
{
	sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	pNext = nullptr;
	flags = 0;
	imageType = VK_IMAGE_TYPE_2D;
	format = VK_FORMAT_UNDEFINED;
	extent = {0, 0, 0};
	mipLevels = 1;
	arrayLayers = 1;
	samples = VK_SAMPLE_COUNT_1_BIT;
	tiling = VK_IMAGE_TILING_OPTIMAL;
	usage = 0;
	sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	queueFamilyIndexCount = 0;
	pQueueFamilyIndices = nullptr;
	initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
}	

ImageCreateInfo& ImageCreateInfo::set_image_type(VkImageType image_type)
{
	this->imageType = image_type;
	return *this;
}

ImageCreateInfo& ImageCreateInfo::set_format(VkFormat format)
{
	this->format = format;
	return *this;
}

ImageCreateInfo& ImageCreateInfo::set_extent(VkExtent3D extent)
{
	this->extent = extent;
	return *this;
}

ImageCreateInfo& ImageCreateInfo::set_mip_levels(uint32_t mip_levels)
{
	this->mipLevels = mip_levels;
	return *this;
}

ImageCreateInfo& ImageCreateInfo::set_array_layers(uint32_t array_layers)
{
	this->arrayLayers = array_layers;
	return *this;
}

ImageCreateInfo& ImageCreateInfo::set_samples(VkSampleCountFlagBits samples)
{
	this->samples = samples;
	return *this;
}

ImageCreateInfo& ImageCreateInfo::set_tiling(VkImageTiling tiling)
{
	this->tiling = tiling;
	return *this;
}

ImageCreateInfo& ImageCreateInfo::set_usage(VkImageUsageFlags usage)
{
	this->usage = usage;
	return *this;
}

ImageCreateInfo& ImageCreateInfo::set_sharing_mode(VkSharingMode sharing_mode)
{
	this->sharingMode = sharing_mode;
	return *this;
}

ImageCreateInfo& ImageCreateInfo::set_queue_family_indices(const std::vector<uint32_t> &queue_family_indices)
{
	this->queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size());
	this->pQueueFamilyIndices = queue_family_indices.data();
	return *this;
}

ImageCreateInfo& ImageCreateInfo::set_initial_layout(VkImageLayout initial_layout)
{
	this->initialLayout = initial_layout;
	return *this;
}

Image::Image(const Device &device, VkImage handle)
		: m_device{device}, m_handle(handle)
{
}

Image::Image(const Device &device, const ImageCreateInfo& info)
		: m_device{device}, m_info(info)
{
	VK_CHECK(vkCreateImage(m_device.get_handle(), &m_info, nullptr, &m_handle));

	vkGetImageMemoryRequirements(m_device.get_handle(), m_handle, &m_memory_requirements);
}

Image::~Image()
{
	if (m_handle != VK_NULL_HANDLE)
	{
		vkDestroyImage(m_device.get_handle(), m_handle, nullptr);
	}
}

const Device &Image::get_device() const
{
	return m_device;
}

VkImage Image::get_handle() const
{
	return m_handle;
}

VkFormat Image::get_format() const
{
	return m_info.format;
}

VkImageType Image::get_image_type() const
{
	return m_info.imageType;
}

uint32_t Image::get_mip_level_count() const
{
	return m_info.mipLevels;
}

uint32_t Image::get_array_layer_count() const
{
	return m_info.arrayLayers;
}

const VkMemoryRequirements &Image::get_memory_requirements() const
{
	return m_memory_requirements;
}

void Image::bind(const DeviceMemory& memory, VkDeviceSize offset)
{
	VK_CHECK(vkBindImageMemory(m_device.get_handle(), m_handle, memory.get_handle(), offset));
}

void Image::upload(const CommandPool &command_pool, const void *data, VkDeviceSize size)
{
	auto stage_buffer = Buffer(m_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	auto stage_memory = DeviceMemory(m_device, stage_buffer.get_memory_requirements(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	stage_buffer.bind(stage_memory);
	stage_memory.copy(0, size, data);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_handle;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = m_info.mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = m_info.arrayLayers;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	auto queue_family_index = m_device.get_physical_device().get_queue_family_index(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
  const auto &queue = m_device.get_queue(queue_family_index, 0);
	utils::submit_commands_to_queue(command_pool, queue, [&](const CommandBuffer &cmd_buffer) {
		vkCmdPipelineBarrier(cmd_buffer.get_handle(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	});

	VkImageSubresourceLayers subresource{};
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource.mipLevel = 0;
	subresource.baseArrayLayer = 0;
	subresource.layerCount = 1;

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource = subresource;
	region.imageOffset = {0, 0, 0};
	region.imageExtent = m_info.extent;

	utils::submit_commands_to_queue(command_pool, queue, [&](const CommandBuffer &cmd_buffer) {
		vkCmdCopyBufferToImage(cmd_buffer.get_handle(), stage_buffer.get_handle(), m_handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	});

	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = 0;

	utils::submit_commands_to_queue(command_pool, queue, [&](const CommandBuffer &cmd_buffer) {
		vkCmdPipelineBarrier(cmd_buffer.get_handle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	});
}