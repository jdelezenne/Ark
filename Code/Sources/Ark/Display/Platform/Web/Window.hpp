#pragma once

#include "Ark/Display/Window.hpp"

namespace Ark::Display::Platform::Web
{
    Window* createWindow(WindowCreateInfo const& info);
}
