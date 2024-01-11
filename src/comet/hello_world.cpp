#include "comet/hello_world.h"

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
    cleanup();
}

void HelloTriangle::initWindow()
{
    glfwInit();

    // 不允许OpenGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // 不允许调整窗口大小
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(WIDTH, HEIGHT, get_name().c_str(), nullptr, nullptr);
}

void HelloTriangle::initVulkan()
{
    createInstance();
}

void HelloTriangle::mainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }
}

void HelloTriangle::cleanup()
{
    glfwDestroyWindow(m_window);

    glfwTerminate();
}

void HelloTriangle::createInstance()
{
    // 指定需要的全局扩展
    std::vector<const char*> extensions;

    // glfw需要的扩展
    uint32_t glfw_ext_count = 0;
    const char **glfw_extensions = glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_ext_count);
    extensions.insert(extensions.end(), glfw_extensions, glfw_extensions + glfw_ext_count);

    m_instance = std::make_unique<Instance>(this->get_name(), extensions);
}