#pragma once

#include "prism/platform/window.h"

namespace prism
{

    class GlfwWindow : public Window
    {
    public:
        GlfwWindow(const Window::Properties &properties);

        ~GlfwWindow() override;

        void* get_handle() const override;

        bool should_close() override;

        void process_events() override;

        void close() override;

        std::vector<const char *> get_required_extensions() const override;

        VkSurfaceKHR create_surface(const Instance& instance) const override;

    private:
        GLFWwindow *m_handle{nullptr};

        std::function<void(uint32_t, uint32_t)> m_resize_callback;
    };

} // namespace prism