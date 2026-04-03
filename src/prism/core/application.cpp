#include "prism/core/application.h"

#include "prism/platform/glfw_window.h"

using namespace prism;

Application *Application::s_instance = nullptr;

Application::Application(const Config &config) {
  s_instance = this;

  // Create window
  m_window = std::make_unique<GlfwWindow>(config.windowProps);
  LOG_INFO("Window created");

  // Create instance
  Instance::ExtensionNames instExtensions = m_window->get_required_extensions();
  instExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  Instance::LayerNames instLayers{"VK_LAYER_KHRONOS_validation"};
  m_instance = std::make_unique<Instance>(instExtensions, instLayers);
  LOG_INFO("Vulkan instance created");

  // Create device
  Device::ExtensionNames dev_exts{};
  dev_exts.push_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
  dev_exts.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
  dev_exts.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
  dev_exts.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  DeviceFeatures dev_features{};
  dev_features.request<VkPhysicalDeviceAccelerationStructureFeaturesKHR, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR>(
      &VkPhysicalDeviceAccelerationStructureFeaturesKHR::accelerationStructure);
  dev_features.request<VkPhysicalDeviceRayQueryFeaturesKHR, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR>(
      &VkPhysicalDeviceRayQueryFeaturesKHR::rayQuery);
  const auto &physical_device = m_instance->pick_physical_device(dev_exts, dev_features);
  m_device = std::make_unique<Device>(physical_device, dev_exts, dev_features);
  LOG_INFO("Vulkan device created");

  // Create surface
  m_surface = std::make_unique<Surface>(*m_instance, *m_window);
  LOG_INFO("Vulkan surface created");

  // Query queue family index
  m_queueFamilyIndex = m_device->get_physical_device().get_queue_family_index(
      VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT, *m_surface);
  LOG_INFO("Vulkan queue family index: {}", m_queueFamilyIndex);

  // Create command pool
  m_cmdPool = std::make_unique<CommandPool>(
      *m_device, m_queueFamilyIndex,
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  LOG_INFO("Vulkan command pool created");
}
