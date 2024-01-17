#pragma once

#include "vulkan/vulkan.h"

namespace comet
{
bool is_depth_only_format(VkFormat format);

bool is_depth_stencil_format(VkFormat format);

bool is_depth_format(VkFormat format);

} // namespace comet