#include "prism/vulkan/image.h"

#include "prism/vulkan/buffer.h"
#include "prism/vulkan/command_buffer.h"
#include "prism/vulkan/device_memory.h"
#include "prism/vulkan/utils.h"

using namespace prism;

VkAccessFlags access_flags(VkImageLayout layout) {
  switch (layout) {
  case VK_IMAGE_LAYOUT_UNDEFINED:
  case VK_IMAGE_LAYOUT_GENERAL:
  case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
    return 0;
  case VK_IMAGE_LAYOUT_PREINITIALIZED:
    return VK_ACCESS_HOST_WRITE_BIT;
  case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
           VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
    return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
           VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
    return VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
  case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
  case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    return VK_ACCESS_TRANSFER_READ_BIT;
  case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    return VK_ACCESS_TRANSFER_WRITE_BIT;
  default:
    assert(false);
    return 0;
  }
}

VkPipelineStageFlags pipeline_stage_flags(VkImageLayout layout) {
  switch (layout) {
  case VK_IMAGE_LAYOUT_UNDEFINED:
    return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  case VK_IMAGE_LAYOUT_GENERAL:
  case VK_IMAGE_LAYOUT_PREINITIALIZED:
    return VK_PIPELINE_STAGE_HOST_BIT;
  case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
  case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    return VK_PIPELINE_STAGE_TRANSFER_BIT;
  case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
    return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
           VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
    return VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
  case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
           VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
    return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  default:
    assert(false);
    return 0;
  }
}

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

Image::Image(const Device &device, const ImageCreateInfo &info, VkImage handle)
		: m_device(device), m_handle(handle), m_info(info), m_layout(info.initialLayout)
{
}

Image::Image(const Device &device, const ImageCreateInfo& info)
		: m_device(device), m_info(info), m_layout(info.initialLayout)
{
	VK_CHECK(vkCreateImage(m_device.get_handle(), &m_info, nullptr, &m_handle));

	vkGetImageMemoryRequirements(m_device.get_handle(), m_handle, &m_memory_requirements);
}

Image::Image(Image &&other) noexcept
		: m_handle(std::exchange(other.m_handle, VK_NULL_HANDLE)),
			m_device(other.m_device),
			m_info(other.m_info),
			m_memory_requirements(other.m_memory_requirements),
			m_layout(other.m_layout)
{
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

const VkExtent3D &Image::get_extent() const
{
	return m_info.extent;
}

const VkMemoryRequirements &Image::get_memory_requirements() const
{
	return m_memory_requirements;
}

const VkImageLayout &Image::get_layout() const
{
	return m_layout;
}

void Image::bind_memory(const DeviceMemory& memory, VkDeviceSize offset) const
{
	VK_CHECK(vkBindImageMemory(m_device.get_handle(), m_handle, memory.get_handle(), offset));
}

void Image::upload(const CommandPool &command_pool, const void *src_data, VkDeviceSize size, VkImageLayout target_layout) const
{
	auto stage_buffer = Buffer(m_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	auto stage_memory = DeviceMemory(m_device, stage_buffer.get_memory_requirements(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	stage_buffer.bind_memory(stage_memory);
	stage_memory.upload(0, size, src_data);

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

	auto queue_family_index = m_device.get_physical_device().get_queue_family_index(VK_QUEUE_TRANSFER_BIT);
  const auto &queue = m_device.get_queue(queue_family_index, 0);
	utils::submit_commands_to_queue(command_pool, queue, [&](const CommandBuffer &cmd_buffer) {
		cmd_buffer.pipeline_barrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, {}, {}, {barrier});
	});

	utils::submit_commands_to_queue(command_pool, queue, [&](const CommandBuffer &cmd_buffer) {
		
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = m_info.arrayLayers;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = m_info.extent;

		cmd_buffer.copy_buffer_to_image(stage_buffer, *this, {region});
	});

	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = target_layout;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = 0;
	utils::submit_commands_to_queue(command_pool, queue, [&](const CommandBuffer &cmd_buffer) {	
		cmd_buffer.pipeline_barrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, {}, {}, {barrier});
	});
}

void Image::download(const CommandPool &command_pool, void *dst_data, VkDeviceSize size, VkImageLayout target_layout) const
{
	auto stage_buffer = Buffer(m_device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	auto stage_memory = DeviceMemory(m_device, stage_buffer.get_memory_requirements(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	stage_buffer.bind_memory(stage_memory);
	
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_handle;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = m_info.mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = m_info.arrayLayers;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

	auto queue_family_index = m_device.get_physical_device().get_queue_family_index(VK_QUEUE_TRANSFER_BIT);
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
		vkCmdCopyImageToBuffer(cmd_buffer.get_handle(), m_handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stage_buffer.get_handle(), 1, &region);
	});

	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	barrier.newLayout = target_layout;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	barrier.dstAccessMask = 0;

	utils::submit_commands_to_queue(command_pool, queue, [&](const CommandBuffer &cmd_buffer) {
		vkCmdPipelineBarrier(cmd_buffer.get_handle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	});

	stage_memory.download(0, size, dst_data);
}

void Image::set_layout(const CommandBuffer &cmd_buffer, VkImageLayout new_layout, VkImageSubresourceRange subresource_range)
{
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = m_layout;
	barrier.newLayout = new_layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_handle;
	barrier.subresourceRange = subresource_range;
	barrier.srcAccessMask = access_flags(m_layout);
	barrier.dstAccessMask = access_flags(new_layout);

	auto src_stage = pipeline_stage_flags(m_layout);
	auto dst_stage = pipeline_stage_flags(new_layout);
	cmd_buffer.pipeline_barrier(src_stage, dst_stage, 0, {}, {}, {barrier});

	m_layout = new_layout;
}

void Image::set_layout(const CommandBuffer &cmd_buffer, VkImageLayout new_layout, VkImageAspectFlags aspect)
{
	VkImageSubresourceRange subresource_range{};
	subresource_range.aspectMask = aspect;
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = get_mip_level_count();
	subresource_range.baseArrayLayer = 0;
	subresource_range.layerCount = get_array_layer_count();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = m_layout;
	barrier.newLayout = new_layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_handle;
	barrier.subresourceRange = subresource_range;
	barrier.srcAccessMask = access_flags(m_layout);
	barrier.dstAccessMask = access_flags(new_layout);

	auto src_stage = pipeline_stage_flags(m_layout);
	auto dst_stage = pipeline_stage_flags(new_layout);
	cmd_buffer.pipeline_barrier(src_stage, dst_stage, 0, {}, {}, {barrier});

	m_layout = new_layout;
}