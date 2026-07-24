#pragma once

#include "Ark/Storage/Path.hpp"

namespace Ark::System
{
    /// Platform-abstracted well-known user/system folders.
    enum class KnownFolder
    {
        Home,
        Desktop,
        Documents,
        Downloads,
        Music,
        Pictures,
        Videos,
        AppDataLocal,
        AppDataRoaming,
        Temp,
    };

    /// Returns the path for a well-known folder.
    /// @param folder Folder identifier.
    /// @return Resolved folder path (or empty path when unavailable).
    Storage::Path getKnownFolderPath(KnownFolder folder);
}
