#include "prism/vulkan/instance.h"

#include "prism/vulkan/utils.h"

using namespace prism;

VKAPI_ATTR VkBool32 VKAPI_CALL _debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                              const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                                                              void *user_data)
{
  if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
  {
    LOG_ERROR("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
  }
  else if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
  {
    LOG_WARN("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
  }
  else if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
  {
    LOG_INFO("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
  }
  else if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
  {
    LOG_TRACE("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
  }
  return VK_FALSE;
}

Instance::Instance(const ExtensionNames &extensions, const LayerNames& layers)
{
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Application";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_MAKE_API_VERSION(1, 3, 0, 0);

  uint32_t available_layers_count = 0;
  vkEnumerateInstanceLayerProperties(&available_layers_count, nullptr);
  std::vector<VkLayerProperties> available_layers(available_layers_count);
  vkEnumerateInstanceLayerProperties(&available_layers_count, available_layers.data());
  assert(utils::check_layers_support(layers, available_layers));
  
  uint32_t available_extensions_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions_count, nullptr);
  std::vector<VkExtensionProperties> available_extensions(available_extensions_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions_count, available_extensions.data());
  assert(utils::check_extensions_support(extensions, available_extensions));

  VkInstanceCreateInfo instance_info{};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;
  instance_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
  instance_info.ppEnabledLayerNames = layers.data();
  instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  instance_info.ppEnabledExtensionNames = extensions.data();

  VK_CHECK(vkCreateInstance(&instance_info, nullptr, &m_handle));

  volkLoadInstance(m_handle);

  query_physical_devices();

  m_extension_functions = std::make_unique<InstanceExtensionFunctions>(this);

  create_debug_messenger();
}

Instance::~Instance()
{
  m_physical_devices.clear();

  if (m_handle)
  {
    vkDestroyInstance(m_handle, nullptr);
  }
}

VkInstance Instance::get_handle() const
{
  return m_handle;
}

const PhysicalDevice& Instance::pick_physical_device() const
{
  for (const auto &device : m_physical_devices)
  {
    if (device->get_properties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      return *device;
    }
  }

  // FIXME: Should we return the first physical device if no discrete GPU found?
  LOG_ERROR("No discrete GPU found");
  return *m_physical_devices.front();
}

void Instance::query_physical_devices()
{
  unsigned int device_count = 0;
  vkEnumeratePhysicalDevices(m_handle, &device_count, nullptr);
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(m_handle, &device_count, devices.data());

  for (const auto &device : devices)
  {
    m_physical_devices.push_back(std::make_unique<PhysicalDevice>(device));
  }
}

void Instance::create_debug_messenger()
{
  if (m_extension_functions->create_debug_utils_messenger)
  {
    VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_info{};
    debug_utils_messenger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_utils_messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_utils_messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_utils_messenger_info.pfnUserCallback = _debug_utils_messenger_callback;

    VK_CHECK(m_extension_functions->create_debug_utils_messenger(m_handle, &debug_utils_messenger_info, nullptr, &m_debug_utils_messenger));
  }
}