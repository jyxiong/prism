#pragma once

#include "volk.h"

namespace comet
{
  VkAccessFlags access_flags(VkImageLayout layout);

  VkPipelineStageFlags pipeline_stage_flags(VkImageLayout layout);

}