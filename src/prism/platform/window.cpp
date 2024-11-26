#include "prism/platform/window.h"

using namespace prism;

Window::Window(const Properties &properties)
    : m_properties(properties)
{
}

glm::uvec2 Window::resize(const glm::uvec2 &extent)
{
    if (m_properties.resizable)
    {
        m_properties.extent = extent;
    }

    return m_properties.extent;
}

const glm::uvec2 &Window::get_extent() const
{
    return m_properties.extent;
}