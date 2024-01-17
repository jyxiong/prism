#pragma once

#include <unordered_set>

#include "volk.h"
#include "comet/vulkan/device.h"

namespace comet
{

    class ImageView;

    class Image
    {
    public:
        Image(Device const &device,
              VkImage handle,
              const VkExtent3D &extent,
              VkFormat format,
              VkImageUsageFlags image_usage,
              VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT);

        ~Image() = default;

        Device &get_device() const;

        VkImage get_handle() const;

        VkFormat get_format() const;

        VkImageSubresource get_subresource() const;

        std::unordered_set<ImageView *> &get_views();

    private:
        Device *m_device{};

        VkImage m_handle{};

        VkImageType m_type{};

        VkExtent3D m_extent{};

        VkFormat m_format{};

        VkImageSubresource m_subresource{};

        /// Image views referring to this image
        std::unordered_set<ImageView *> m_views;
    };
} // namespace comet