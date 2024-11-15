#include "prism/platform/window.h"

using namespace prism;

Window::Window(const Properties &properties)
    : m_properties(properties)
{
}

glm::ivec2 Window::resize(const glm::ivec2 &extent)
{
    if (m_properties.resizable)
    {
        m_properties.extent = extent;
    }

    return m_properties.extent;
}

const glm::ivec2 &Window::get_extent() const
{
    return m_properties.extent;
}