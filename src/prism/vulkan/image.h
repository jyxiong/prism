#pragma once

#include "prism/vulkan/device.h"
#include "prism/vulkan/device_memory.h"
#include "prism/vulkan/command_pool.h"

namespace prism
{
  class ImageCreateInfo : public VkImageCreateInfo
  {
    public:
      ImageCreateInfo();

      ImageCreateInfo& set_image_type(VkImageType image_type);

      ImageCreateInfo& set_format(VkFormat format);

      ImageCreateInfo& set_extent(VkExtent3D extent);

      ImageCreateInfo& set_mip_levels(uint32_t mip_levels);

      ImageCreateInfo& set_array_layers(uint32_t array_layers);

      ImageCreateInfo& set_samples(VkSampleCountFlagBits samples);

      ImageCreateInfo& set_tiling(VkImageTiling tiling);

      ImageCreateInfo& set_usage(VkImageUsageFlags usage);

      ImageCreateInfo& set_sharing_mode(VkSharingMode sharing_mode);

      ImageCreateInfo& set_queue_family_indices(const std::vector<uint32_t> &queue_family_indices);

      ImageCreateInfo& set_initial_layout(VkImageLayout initial_layout);
  };

  class Image
  {
  public:
    Image(const Device &device, const ImageCreateInfo &info, VkImage handle);

    Image(const Device &device, const ImageCreateInfo &info);

    Image(const Image &) = delete;

    Image(Image &&other) noexcept;

    virtual ~Image();

    Image &operator=(const Image &) = delete;

    Image &operator=(Image &&) = delete;

    const Device &get_device() const;

    VkImage get_handle() const;

    VkFormat get_format() const;

    VkImageType get_image_type() const;

    uint32_t get_mip_level_count() const;

    uint32_t get_array_layer_count() const;

    const VkExtent3D &get_extent() const;

    const VkMemoryRequirements &get_memory_requirements() const;

    void bind_memory(const DeviceMemory& memory, VkDeviceSize offset = 0) const;

    void upload(const CommandPool &command_pool, const void *data, VkDeviceSize size, VkImageLayout target_layout = VK_IMAGE_LAYOUT_GENERAL) const;

    void download(const CommandPool &command_pool, void *data, VkDeviceSize size, VkImageLayout target_layout = VK_IMAGE_LAYOUT_GENERAL) const;

    void set_layout(const CommandBuffer &cmd_buffer, VkImageLayout new_layout, VkImageSubresourceRange subresource_range);

  protected:
    const Device &m_device;

    VkImage m_handle{};

    ImageCreateInfo m_info;

    VkMemoryRequirements m_memory_requirements;

    VkImageLayout m_layout;
  };
} // namespace prism