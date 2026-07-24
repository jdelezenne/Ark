#pragma once

#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/Display/Window.hpp"

namespace Ark::Display::Platform::Windows
{
    Window* createWindow(WindowCreateInfo const& info);
}

#endif
