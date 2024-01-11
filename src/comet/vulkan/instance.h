#pragma once

#include <string>
#include <vector>

#include "vulkan/vulkan.h"

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

        VkInstance getHandle() const { return m_handle; }

    private:
        VkInstance m_handle;

        VkDebugUtilsMessengerEXT m_debug_utils_messenger;
    };
} // namespace comet