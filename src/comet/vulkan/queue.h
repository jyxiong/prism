#pragma once

#include "volk.h"

namespace comet
{
    class Device;

    class Queue
    {
        public:
            Queue(Device &device, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present, uint32_t index);
            ~Queue();

            VkQueue get_handle() const;

            const VkQueueFamilyProperties &get_properties() const;

            VkBool32 support_present() const;

        private:
            const Device& m_device;

            VkQueue m_handle;

            uint32_t m_family_index;

            uint32_t m_index;

            VkBool32 m_can_present;

            VkQueueFamilyProperties m_properties;

    }; // class Queue
} // namespace comet