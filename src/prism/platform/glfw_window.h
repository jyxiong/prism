#pragma once

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

        VkSurfaceKHR create_surface(const Instance& instance) const override;

    private:
        GLFWwindow *m_handle{nullptr};
    };

} // namespace prism