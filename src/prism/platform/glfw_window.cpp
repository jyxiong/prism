#include "prism/platform/glfw_window.h"

#include "prism/vulkan/instance.h"

using namespace prism;

GlfwWindow::GlfwWindow(const Window::Properties &properties)
    : Window(properties)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // 不允许OpenGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // 不允许调整窗口大小
    glfwWindowHint(GLFW_RESIZABLE, properties.resizable);

    m_handle = glfwCreateWindow(properties.extent.x, properties.extent.y, properties.title.c_str(), nullptr, nullptr);
}

GlfwWindow::~GlfwWindow()
{
    glfwDestroyWindow(m_handle);

    glfwTerminate();
}

void* GlfwWindow::get_handle() const
{
    return m_handle;
}

bool GlfwWindow::should_close()
{
    return glfwWindowShouldClose(m_handle);
}

void GlfwWindow::process_events()
{
    glfwPollEvents();
}

void GlfwWindow::close()
{
    glfwSetWindowShouldClose(m_handle, GLFW_TRUE);
}

std::vector<const char *> GlfwWindow::get_required_extensions() const
{
    uint32_t glfw_extension_count{0};
    const char **names = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    return {names, names + glfw_extension_count};
}

VkSurfaceKHR GlfwWindow::create_surface(const Instance& instance) const
{
    VkSurfaceKHR surface{nullptr};
    if (glfwCreateWindowSurface(instance.get_handle(), m_handle, nullptr, &surface) != VK_SUCCESS)
    {
        LOG_CRITICAL("Failed to create window surface!");
        return VK_NULL_HANDLE;
    }
    return surface;
}