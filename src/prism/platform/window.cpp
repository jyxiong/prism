#include "prism/platform/window.h"

using namespace prism;

Window::Window(const Properties &properties)
    : m_properties(properties)
{
}

Window::Extent Window::resize(const Extent &extent)
{
    if (m_properties.resizable)
    {
        m_properties.extent.width = extent.width;
        m_properties.extent.height = extent.height;
    }

    return m_properties.extent;
}