#include <stdexcept>
#include <iostream>

#include "GLFW/glfw3.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

class HelloTriangleApplication
{
public:
    void run()
    {
        initWindow();

        initVulkan();

        mainLoop();

        cleanup();
    }

private:
    void initWindow()
    {
        glfwInit();

        // 禁止GLFW创建OpenGL上下文
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // 禁止窗口大小改变
        glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

        m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    }

    void initVulkan()
    {

    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(m_window))
        {
            glfwPollEvents();
        }
    }

    void cleanup()
    {
        glfwDestroyWindow(m_window);

        glfwTerminate();
    }


private:
    GLFWwindow* m_window;
};

int main()
{
    HelloTriangleApplication app{};

    try
    {
        app.run();
    } catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}