#pragma once

namespace prism
{
  class Instance;

  class InstanceExtensionFunctions
  {
  public:
    InstanceExtensionFunctions(Instance *instance);

    // VK_EXT_debug_utils
    PFN_vkSetDebugUtilsObjectNameEXT set_debug_utils_object_name = nullptr;
    PFN_vkSetDebugUtilsObjectTagEXT set_debug_utils_object_tag = nullptr;
    PFN_vkQueueBeginDebugUtilsLabelEXT queue_begin_debug_utils_label = nullptr;
    PFN_vkQueueEndDebugUtilsLabelEXT queue_end_debug_utils_label = nullptr;
    PFN_vkQueueInsertDebugUtilsLabelEXT queue_insert_debug_utils_label = nullptr;
    PFN_vkCmdBeginDebugUtilsLabelEXT cmd_begin_debug_utils_label = nullptr;
    PFN_vkCmdEndDebugUtilsLabelEXT cmd_end_debug_utils_label = nullptr;
    PFN_vkCmdInsertDebugUtilsLabelEXT cmd_insert_debug_utils_label = nullptr;
    PFN_vkCreateDebugUtilsMessengerEXT create_debug_utils_messenger = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_debug_utils_messenger = nullptr;
    PFN_vkSubmitDebugUtilsMessageEXT submit_debug_utils_message = nullptr;

    // VK_EXT_calibrated_timestamps
    PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT get_physical_device_calibrateable_time_domains = nullptr;
    PFN_vkGetCalibratedTimestampsEXT get_calibrated_timestamps = nullptr;
  };

} // namespace prism