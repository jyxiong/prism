#include "comet/vulkan/instance.h"

#include "comet/vulkan/error.h"

using namespace comet;

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
  app_info.pApplicationName = "LilyApplication";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "LilyEngine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_MAKE_API_VERSION(1, 3, 0, 0);

  assert(check_layer_support(layers));
  assert(check_extension_support(extensions));

  VkInstanceCreateInfo instance_info{};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;
  instance_info.enabledLayerCount = static_cast<uint32_t>(m_layers.size());
  instance_info.ppEnabledLayerNames = m_layers.data();
  instance_info.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
  instance_info.ppEnabledExtensionNames = m_extensions.data();

  VK_CHECK(vkCreateInstance(&instance_info, nullptr, &m_handle));

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

  LOG_ERROR("No discrete GPU found");
  return PhysicalDevice{VK_NULL_HANDLE};
}

bool Instance::check_layer_support(const std::vector<std::string> &reqiured_layers)
{
  uint32_t count = 0;
  vkEnumerateInstanceLayerProperties(&count, nullptr);
  std::vector<VkLayerProperties> available_layers(count);
  vkEnumerateInstanceLayerProperties(&count, available_layers.data());

  for (const auto &reqiured_layer : reqiured_layers)
  {
    auto found = std::find_if(available_layers.begin(), available_layers.end(), [&](const VkLayerProperties &layer)
                              { return strcmp(layer.layerName, reqiured_layer.c_str()) == 0; });
    if (found != available_layers.end())
    {
      m_layers.push_back(reqiured_layer.c_str());
    }
    else
    {
      LOG_ERROR("Required instance layer not found: {}", reqiured_layer);
      return false;
    }
  }

  return true;
}

bool Instance::check_extension_support(const std::vector<std::string> &reqiured_extensions)
{
  uint32_t count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> available_extensions(count);
  vkEnumerateInstanceExtensionProperties(nullptr, &count, available_extensions.data());

  for (const auto &reqiured_extension : reqiured_extensions)
  {
    auto found = std::find_if(available_extensions.begin(), available_extensions.end(), [&](const VkExtensionProperties &extension)
                              { return strcmp(extension.extensionName, reqiured_extension.c_str()) == 0; });
    if (found != available_extensions.end())
    {
      m_extensions.push_back(reqiured_extension.c_str());
    }
    else
    {
      LOG_ERROR("Required instance extension not found: {}", reqiured_extension);
      return false;
    }
  }

  return true;
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