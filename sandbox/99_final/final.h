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
#include "comet/vulkan/swapchain.h"
#include "comet/vulkan/image_view.h"

class HelloTriangleApplication
{
private:
    std::unique_ptr<comet::Window> m_window;
    VkSurfaceKHR m_surface;
    std::unique_ptr<comet::Instance> m_instance;
    std::unique_ptr<comet::Device> m_device;
    std::unique_ptr<comet::Swapchain> m_swapchain;
    std::vector<comet::ImageView> m_image_views;
public:
    void run();

    ~HelloTriangleApplication();

private:
    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

    std::vector<const char *> getRequiredInstanceExtensions();

    std::vector<const char *> getRequiredDeviceExtensions();
};
