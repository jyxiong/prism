#pragma once

#include <stdexcept>
#include <iostream>
#include <functional>
#include <vector>
#include <optional>

#include <memory>

#include "comet/platform/window/glfw_window.h"
#include "comet/vulkan/instance.h"
#include "comet/vulkan/physical_device.h"

struct QueueFamilyIndices
{
    std::optional<unsigned int> graphicsFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value();
    }
};

class HelloTriangleApplication
{
private:
    std::unique_ptr<comet::Window> m_window;
    std::unique_ptr<comet::Instance> m_instance;
    std::shared_ptr<comet::PhysicalDevice> m_physicalDevice;

public:
    void run();

private:
    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

    void createInstance();

    std::vector<const char *> getRequiredExtensions();

    void pickPhysicalDevice();

    bool isDeviceSuitable(const std::shared_ptr<comet::PhysicalDevice>& device);

    QueueFamilyIndices findQueueFamilies(const std::shared_ptr<comet::PhysicalDevice>& device);
};
