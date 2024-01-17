#pragma once

#include <string>
#include <vector>

#include "volk.h"

namespace comet
{
    class Instance
    {
    public:
        Instance(const std::string &app_name, const std::vector<const char*> &extensions);
        Instance(const Instance &) = delete;

        Instance(Instance &&) = delete;

        ~Instance();

        Instance &operator=(const Instance &) = delete;

        Instance &operator=(Instance &&) = delete;

        VkInstance getHandle() const { return m_handle; }

    private:
        VkInstance m_handle;
    };
} // namespace comet