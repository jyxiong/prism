#include "vk_engine.h"

void VulkanEngine::init()
{
    // 初始化GLFW
    glfwInit();
    // 不创建OpenGL上下文
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // 窗口大小不可变
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // 创建窗口
    m_window = glfwCreateWindow((int) m_windowExtent.width, (int) m_windowExtent.height,
                                "Vulkan Engine", nullptr, nullptr);

    // 初始化完成
    m_isInitialized = true;
}

void VulkanEngine::cleanup()
{
    if (m_isInitialized)
    {
        // 销毁窗口
        glfwDestroyWindow(m_window);
        // 终止GLFW
        glfwTerminate();
    }
}

void VulkanEngine::draw()
{

}

void VulkanEngine::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }

    draw();
}
