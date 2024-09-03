#include "prism/core/application.h"

using namespace prism;

Application::Application(const std::string& name)
    : m_name(name)
{
}

const std::string& Application::get_name() const
{
    return m_name;
}