#pragma once

#include <string>
#include <vector>
#include <memory>

#include "vulkan/vulkan.h"

#include "comet/vulkan/physical_device.h"

namespace comet
{
    class Instance
    {
    public:
        Instance(const std::string &app_name, const std::vector<const char *> &extensions, const std::vector<const char *> &validation_layers);
        Instance(const Instance &) = delete;

        Instance(Instance &&) = delete;

        ~Instance();

        Instance &operator=(const Instance &) = delete;

        Instance &operator=(Instance &&) = delete;

        VkInstance get_handle() const;

        const std::vector<std::shared_ptr<PhysicalDevice>> &get_physical_devices() const;

        PhysicalDevice &get_suitable_gpu(VkSurfaceKHR);

    private:
        void query_physical_devices();

    private:
        VkInstance m_handle;

        VkDebugUtilsMessengerEXT m_debug_utils_messenger;

        std::vector<std::shared_ptr<PhysicalDevice>> m_physical_devices;
    };
} // namespace comet