#pragma once

#include "GLFW/glfw3.h"

#include "vk_type.h"

class VulkanEngine
{
private:
    bool m_isInitialized{ false };

    VkExtent2D m_windowExtent{1280, 960};

    GLFWwindow *m_window{ nullptr };

public:
    // 初始化引擎
    void init();

    // 清除引擎
    void cleanup();

    // 绘制循环
    void draw();

    // 运行循环
    void run();

}; // class VulkanEngine