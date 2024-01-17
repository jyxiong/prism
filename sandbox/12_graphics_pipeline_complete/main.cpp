#include <stdexcept>
#include <iostream>
#include <cstdlib>

#include "12_graphics_pipeline_complete.h"

int main()
{
    HelloTriangleApplication app{};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}