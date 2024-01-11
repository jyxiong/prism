#pragma once

#include <stdexcept>
#include <iostream>
#include <functional>
#include <memory>

#include "vulkan/vulkan.h"

#include "comet/core/application.h"
#include "comet/core/window.h"
#include "comet/vulkan/instance.h"

namespace comet
{
    const unsigned int WIDTH = 800;
    const unsigned int HEIGHT = 600;

    class HelloTriangle : public Application
    {
    public:
        HelloTriangle(const std::string &name);

        ~HelloTriangle() override = default;

        void run();

    private:
        void initWindow();

        void initVulkan();

        void mainLoop();

        void cleanup();

        void createInstance();

    private:
        std::unique_ptr<Window> m_window{nullptr};
        std::unique_ptr<Instance> m_instance{nullptr};
    };

}
