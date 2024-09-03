#include "prism/vulkan/instance_extension_functions.h"

#include "prism/vulkan/instance.h"

using namespace prism;

InstanceExtensionFunctions::InstanceExtensionFunctions(Instance* instance)
{
    set_debug_utils_object_name = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkSetDebugUtilsObjectNameEXT"));
    set_debug_utils_object_tag = reinterpret_cast<PFN_vkSetDebugUtilsObjectTagEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkSetDebugUtilsObjectTagEXT"));
    queue_begin_debug_utils_label = reinterpret_cast<PFN_vkQueueBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkQueueBeginDebugUtilsLabelEXT"));
    queue_end_debug_utils_label = reinterpret_cast<PFN_vkQueueEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkQueueEndDebugUtilsLabelEXT"));
    queue_insert_debug_utils_label = reinterpret_cast<PFN_vkQueueInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkQueueInsertDebugUtilsLabelEXT"));
    cmd_begin_debug_utils_label = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkCmdBeginDebugUtilsLabelEXT"));
    cmd_end_debug_utils_label = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkCmdEndDebugUtilsLabelEXT"));
    cmd_insert_debug_utils_label = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkCmdInsertDebugUtilsLabelEXT"));
    create_debug_utils_messenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkCreateDebugUtilsMessengerEXT"));
    destroy_debug_utils_messenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkDestroyDebugUtilsMessengerEXT"));
    submit_debug_utils_message = reinterpret_cast<PFN_vkSubmitDebugUtilsMessageEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkSubmitDebugUtilsMessageEXT"));

    get_physical_device_calibrateable_time_domains = reinterpret_cast<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
    get_calibrated_timestamps = reinterpret_cast<PFN_vkGetCalibratedTimestampsEXT>(vkGetInstanceProcAddr(instance->get_handle(), "vkGetCalibratedTimestampsEXT"));
}