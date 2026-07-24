#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/// Set a callback to receive window input events from the Cocoa window
/// @param callback Function pointer that receives NSEvent* as void*
/// @note This is used by input drivers to receive events directly from the window
///       without requiring global event monitoring permissions
void Ark_Display_MacOS_SetInputDriverEventCallback(void (*callback)(void* event));

#ifdef __cplusplus
}
#endif

namespace Ark::Display::Platform::MacOS
{
    /// Set a callback to receive window input events from the Cocoa window (C++ wrapper)
    inline void setInputDriverEventCallback(void (*callback)(void* event))
    {
        Ark_Display_MacOS_SetInputDriverEventCallback(callback);
    }
}
