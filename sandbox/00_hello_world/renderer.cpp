#include "renderer.h"

#include "stb_image_write.h"

#include "prism/vulkan/device_features.h"

static constexpr uint32_t WIDTH = 800;
static constexpr uint32_t HEIGHT = 600;
static constexpr uint32_t CHANNEl = 3;
using value_type = float;
static value_type fill_value = 0.5f;

Renderer::Renderer()
{
  create_instance();
  create_device();
  create_framebuffer();
  create_command_pool();
  create_command_buffer();
}

void Renderer::render() {
  m_command_buffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  vkCmdFillBuffer(m_command_buffer->get_handle(), m_framebuffer->get_handle(),
                  0, WIDTH * HEIGHT * CHANNEl * sizeof(value_type),
                  *reinterpret_cast<uint32_t *>(&fill_value));

  m_command_buffer->end();

  auto &queue = m_device->get_queue(m_queue_family_index, 0);
  queue.submit(*m_command_buffer);
  queue.wait_idle();
}

void Renderer::save(const std::string &path) {

  value_type *data;
  m_framebuffer_memory->map(0, WIDTH * HEIGHT * CHANNEl * sizeof(value_type), 0,
                            reinterpret_cast<void **>(&data));

  stbi_write_hdr(path.c_str(), WIDTH, HEIGHT, CHANNEl, data);

  m_framebuffer_memory->unmap();
}

void Renderer::create_instance() {
  Instance::ExtensionNames inst_exts{};
  inst_exts.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  Instance::LayerNames inst_layers{};
  inst_layers.push_back("VK_LAYER_KHRONOS_validation");
  m_instance = std::make_unique<Instance>(inst_exts, inst_layers);
}

void Renderer::create_device() {
  Device::ExtensionNames dev_exts{};
  dev_exts.push_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
  dev_exts.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
  dev_exts.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);

  DeviceFeatures dev_features{};
  dev_features.request<VkPhysicalDeviceAccelerationStructureFeaturesKHR>(
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
      &VkPhysicalDeviceAccelerationStructureFeaturesKHR::accelerationStructure);
  dev_features.request<VkPhysicalDeviceRayQueryFeaturesKHR>(
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
      &VkPhysicalDeviceRayQueryFeaturesKHR::rayQuery);
  m_device = std::make_unique<Device>(m_instance->pick_physical_device(),
                                      dev_exts, dev_features);
  m_queue_family_index = m_device->get_physical_device().get_queue_family_index(
      VK_QUEUE_TRANSFER_BIT);
}

void Renderer::create_framebuffer() {
  m_framebuffer = std::make_unique<Buffer>(
      *m_device, WIDTH * HEIGHT * CHANNEl * sizeof(value_type),
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  auto memory_requirements = m_framebuffer->get_memory_requirements();
  m_framebuffer_memory = std::make_unique<DeviceMemory>(
      *m_device, memory_requirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  m_framebuffer->bind(*m_framebuffer_memory);
}

void Renderer::create_command_pool() {
  m_command_pool =
      std::make_unique<CommandPool>(*m_device, m_queue_family_index);
}

void Renderer::create_command_buffer() {
  m_command_buffer = std::make_unique<CommandBuffer>(*m_command_pool);
}
