#pragma once

#if defined(ARK_PLATFORM_MACOS)

#include "Ark/Display/Window.hpp"

namespace Ark::Display::Platform::MacOS
{
    Window* createWindow(WindowCreateInfo const& info);
}

#endif
