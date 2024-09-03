#include "comet/vulkan/common.h"

namespace comet
{

bool is_depth_only_format(VkFormat format)
{
    return format == VK_FORMAT_D16_UNORM ||
           format == VK_FORMAT_D32_SFLOAT;
}

bool is_depth_stencil_format(VkFormat format)
{
    return format == VK_FORMAT_D16_UNORM_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT ||
           format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}

bool is_depth_format(VkFormat format)
{
    return is_depth_only_format(format) || is_depth_stencil_format(format);
}

}
