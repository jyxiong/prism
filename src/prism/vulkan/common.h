#pragma once


namespace prism
{
  VkAccessFlags access_flags(VkImageLayout layout);

  VkPipelineStageFlags pipeline_stage_flags(VkImageLayout layout);

}