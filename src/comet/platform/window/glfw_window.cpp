#include "comet/platform/window/glfw_window.h"

#include <stdexcept>

using namespace comet;

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

    m_handle = glfwCreateWindow(properties.extent.width, properties.extent.height, properties.title.c_str(), nullptr, nullptr);
}

GlfwWindow::~GlfwWindow()
{
    glfwDestroyWindow(m_handle);

    glfwTerminate();
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