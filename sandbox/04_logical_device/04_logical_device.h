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
#include "comet/vulkan/device.h"

class HelloTriangleApplication
{
private:
    std::unique_ptr<comet::Window> m_window;
    std::unique_ptr<comet::Instance> m_instance;
    std::unique_ptr<comet::Device> m_device;

    VkQueue m_graphicsQueue;

public:
    void run();

private:
    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

    std::vector<const char *> getRequiredExtensions();
};
