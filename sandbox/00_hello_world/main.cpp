#include "stb_image_write.h"

#include "comet/vulkan/buffer.h"
#include "comet/vulkan/command_buffer.h"
#include "comet/vulkan/command_pool.h"
#include "comet/vulkan/device.h"
#include "comet/vulkan/device_features.h"
#include "comet/vulkan/device_memory.h"
#include "comet/vulkan/instance.h"

using namespace comet;

static constexpr uint32_t WIDTH = 800;
static constexpr uint32_t HEIGHT = 600;
static constexpr uint32_t CHANNEl = 3;
using value_type = float;
static value_type fill_value = 0.5f;

int main() {
  if (volkInitialize()) {
    throw std::runtime_error("Failed to initialize volk.");
  }
  Instance::ExtensionNames inst_exts{};
  inst_exts.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  Instance::LayerNames inst_layers{};
  inst_layers.push_back("VK_LAYER_KHRONOS_validation");
  auto instance = std::make_unique<Instance>(inst_exts, inst_layers);

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
  auto device = std::make_unique<Device>(instance->pick_physical_device(),
                                         dev_exts, dev_features);
  auto queue_family_index =
      device->get_physical_device().get_queue_family_index(
          VK_QUEUE_TRANSFER_BIT);

  auto buffer = std::make_unique<Buffer>(
      *device, WIDTH * HEIGHT * CHANNEl * sizeof(value_type),
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
  auto memory_requirements = buffer->get_memory_requirements();
  auto memory = std::make_unique<DeviceMemory>(
      *device, memory_requirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  buffer->bind(*memory);

  auto cmd_pool = std::make_unique<CommandPool>(*device, queue_family_index);
  auto cmd_buffer = std::make_unique<CommandBuffer>(*cmd_pool);
  cmd_buffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  vkCmdFillBuffer(cmd_buffer->get_handle(), buffer->get_handle(), 0,
                  WIDTH * HEIGHT * CHANNEl * sizeof(value_type),
                  *reinterpret_cast<uint32_t *>(&fill_value));

  cmd_buffer->end();

  auto &queue = device->get_queue(queue_family_index, 0);
  queue.submit(*cmd_buffer);

  queue.wait_idle();

  value_type *data;
  memory->map(0, WIDTH * HEIGHT * CHANNEl * sizeof(value_type), 0,
              reinterpret_cast<void **>(&data));

  stbi_write_hdr("output.hdr", WIDTH, HEIGHT, CHANNEl, data);

  memory->unmap();

  return 0;
}