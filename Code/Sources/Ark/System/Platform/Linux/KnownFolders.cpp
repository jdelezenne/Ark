#if defined(ARK_PLATFORM_LINUX)

#include "Ark/System/KnownFolders.hpp"

#include <cstdlib>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

namespace Ark::System
{
    static String getHomeDir()
    {
        char const* home = std::getenv("HOME");
        if (home != nullptr)
        {
            return home;
        }

        passwd* pw = getpwuid(getuid());
        if (pw != nullptr && pw->pw_dir != nullptr)
        {
            return pw->pw_dir;
        }
        return {};
    }

    Storage::Path getKnownFolderPath(KnownFolder folder)
    {
        String home = getHomeDir();

        switch (folder)
        {
            case KnownFolder::Home:
                return Storage::Path(home);
            case KnownFolder::Desktop:
                return Storage::Path(String::format("{}/Desktop", home));
            case KnownFolder::Documents:
                return Storage::Path(String::format("{}/Documents", home));
            case KnownFolder::Downloads:
                return Storage::Path(String::format("{}/Downloads", home));
            case KnownFolder::Music:
                return Storage::Path(String::format("{}/Music", home));
            case KnownFolder::Pictures:
                return Storage::Path(String::format("{}/Pictures", home));
            case KnownFolder::Videos:
                return Storage::Path(String::format("{}/Videos", home));
            case KnownFolder::AppDataLocal:
                return Storage::Path(String::format("{}/.local/share", home));
            case KnownFolder::AppDataRoaming:
                return Storage::Path(String::format("{}/.config", home));
            case KnownFolder::Temp:
            {
                char const* tmp = std::getenv("TMPDIR");
                if (tmp == nullptr)
                {
                    tmp = "/tmp";
                }
                return Storage::Path(String(tmp));
            }
        }

        return Storage::Path();
    }
}

#endif
