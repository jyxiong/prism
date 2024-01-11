#include "final.h"

#include "comet/platform/window/glfw_window.h"

using namespace comet;

HelloTriangle::HelloTriangle(const std::string &name)
    : Application(name)
{
}

void HelloTriangle::run()
{
    initWindow();
    initVulkan();
    mainLoop();
}

void HelloTriangle::initWindow()
{
    Window::Properties properties;
    properties.title = this->get_name();
    properties.resizable = false;

	if (properties.mode == Window::Mode::Headless)
	{
        // TODO
	}
	else
	{
		m_window = std::make_unique<GlfwWindow>(properties);
	}
}

void HelloTriangle::initVulkan()
{
    createInstance();
}

void HelloTriangle::mainLoop()
{
    while (!m_window->should_close())
    {
        m_window->process_events();
    }
}

void HelloTriangle::cleanup()
{
}

void HelloTriangle::createInstance()
{
    // 指定需要的全局扩展
    std::vector<const char *> extensions;

    // glfw需要的扩展
    auto glfw_extensions = m_window->get_required_surface_extensions();
    extensions.insert(extensions.end(), glfw_extensions.begin(), glfw_extensions.end());

    m_instance = std::make_unique<Instance>(this->get_name(), extensions);
}