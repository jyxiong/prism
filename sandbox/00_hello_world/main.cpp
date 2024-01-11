#include <stdexcept>
#include <iostream>
#include "comet/hello_world.h"

using namespace comet;

int main()
{
    HelloTriangle app("Hello Triangle");

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
