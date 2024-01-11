#include <stdexcept>
#include <iostream>
#include <cstdlib>

#include "06_swap_chain_creation.h"

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