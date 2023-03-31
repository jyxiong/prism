#include <stdexcept>
#include <iostream>
#include <functional>
#include <vector>

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// 指定校验层
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

// 是否启用校验层
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

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
        createInstance();
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
        vkDestroyInstance(m_instance, nullptr);

        glfwDestroyWindow(m_window);

        glfwTerminate();
    }

    void createInstance()
    {
        // 校验
        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        // instance结构体
        VkInstanceCreateInfo createInfo{};

        //
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        //
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        createInfo.pApplicationInfo = &appInfo;

        // 添加校验层信息
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else
        {
            createInfo.enabledLayerCount = 0;
        }

        // 添加需要的扩展
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        // 创建vk实例
        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    bool checkValidationLayerSupport()
    {
        // 获取所有可用的实例校验层数量
        unsigned int layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // 获取所有可用的实例校验层
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        // 检查指定的校验层是否包含在可用校验层中
        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerProperties.layerName, layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    std::vector<const char*> getRequiredExtensions()
    {
        // 获取glfw需要的扩展
        unsigned int glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        // debug需要的扩展
        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

private:
    GLFWwindow* m_window;
    VkInstance  m_instance;
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