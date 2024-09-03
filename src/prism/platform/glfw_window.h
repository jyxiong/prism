#pragma once

#include "volk.h"
#include "GLFW/glfw3.h"

#include "prism/platform/window.h"

namespace prism
{

    class GlfwWindow : public Window
    {
    public:
        GlfwWindow(const Window::Properties &properties);

        ~GlfwWindow() override;

        bool should_close() override;

        void process_events() override;

        void close() override;

        std::vector<const char *> get_required_extensions() const override;

        VkSurfaceKHR create_surface(const Instance& instance) override;

    private:
        GLFWwindow *m_handle{nullptr};
    };

} // namespace prism