#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/System/KnownFolders.hpp"
#include "Ark/Strings/Unicode.hpp"

#include <ShlObj.h>

namespace Ark::System
{
    Storage::Path getKnownFolderPath(KnownFolder folder)
    {
        KNOWNFOLDERID id = FOLDERID_Profile;
        switch (folder)
        {
            case KnownFolder::Home:
                id = FOLDERID_Profile;
                break;
            case KnownFolder::Desktop:
                id = FOLDERID_Desktop;
                break;
            case KnownFolder::Documents:
                id = FOLDERID_Documents;
                break;
            case KnownFolder::Downloads:
                id = FOLDERID_Downloads;
                break;
            case KnownFolder::Music:
                id = FOLDERID_Music;
                break;
            case KnownFolder::Pictures:
                id = FOLDERID_Pictures;
                break;
            case KnownFolder::Videos:
                id = FOLDERID_Videos;
                break;
            case KnownFolder::AppDataLocal:
                id = FOLDERID_LocalAppData;
                break;
            case KnownFolder::AppDataRoaming:
                id = FOLDERID_RoamingAppData;
                break;
            case KnownFolder::Temp:
            {
                wchar_t path[MAX_PATH] = {};
                DWORD len = GetTempPathW(MAX_PATH, path);
                if (len > 0)
                {
                    return Storage::Path(Unicode::fromWide(path));
                }
                return Storage::Path();
            }
        }

        PWSTR wpath = nullptr;
        HRESULT hr = SHGetKnownFolderPath(id, KF_FLAG_DEFAULT, nullptr, &wpath);
        if (FAILED(hr) || wpath == nullptr)
        {
            return Storage::Path();
        }
        String path = Unicode::fromWide(wpath);
        CoTaskMemFree(wpath);
        return Storage::Path(path);
    }
}

#endif
