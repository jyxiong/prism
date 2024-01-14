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
        Instance(const std::string &app_name, const std::vector<const char *> &required_extensions, const std::vector<const char *> &required_layers);

        Instance(const Instance &) = delete;

        Instance(Instance &&) = delete;

        ~Instance();

        Instance &operator=(const Instance &) = delete;

        Instance &operator=(Instance &&) = delete;

        VkInstance get_handle() const;

        const std::vector<std::unique_ptr<PhysicalDevice>> &get_physical_devices() const;

        const PhysicalDevice &get_suitable_physical_device(VkSurfaceKHR);

        bool is_extension_enabled(const char *extension_name) const;

    private:
        void query_physical_devices();

    private:
        VkInstance m_handle;

        VkDebugUtilsMessengerEXT m_debug_utils_messenger;

        std::vector<std::unique_ptr<PhysicalDevice>> m_physical_devices;

        std::vector<const char *> m_enabled_extensions;
    };
} // namespace comet