#pragma once

#include <stdexcept>
#include <iostream>
#include <functional>
#include <memory>

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "comet/application.h"
#include "comet/vulkan/instance.h"

namespace comet
{
    const unsigned int WIDTH = 800;
    const unsigned int HEIGHT = 600;

    class HelloTriangle : public Application
    {
    public:
        HelloTriangle(const std::string &name);

        void run();

    private:
        void initWindow();

        void initVulkan();

        void mainLoop();

        void cleanup();

        void createInstance();

    private:
        GLFWwindow *m_window;
        std::unique_ptr<Instance> m_instance{nullptr};
    };

}
