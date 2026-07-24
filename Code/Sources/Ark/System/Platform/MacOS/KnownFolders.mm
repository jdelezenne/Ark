#if defined(ARK_PLATFORM_MACOS)

#import <Foundation/Foundation.h>
#include <string>

#include "Ark/System/KnownFolders.hpp"
#include "Ark/Strings/String.hpp"

extern "C" char const* arkMacosGetKnownFolderPath(int code)
{
    static thread_local std::string buffer;
    switch (code)
    {
        case 0: // Home
            buffer = [NSHomeDirectory() UTF8String];
            return buffer.c_str();
        case 1: // Desktop
        {
            NSArray<NSString*>* paths = NSSearchPathForDirectoriesInDomains(NSDesktopDirectory, NSUserDomainMask, YES);
            if (paths.count == 0) { return nullptr; }
            buffer = [paths.firstObject UTF8String];
            return buffer.c_str();
        }
        case 2: // Documents
        {
            NSArray<NSString*>* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            if (paths.count == 0) { return nullptr; }
            buffer = [paths.firstObject UTF8String];
            return buffer.c_str();
        }
        case 3: // Downloads
        {
            NSArray<NSString*>* paths = NSSearchPathForDirectoriesInDomains(NSDownloadsDirectory, NSUserDomainMask, YES);
            if (paths.count == 0) { return nullptr; }
            buffer = [paths.firstObject UTF8String];
            return buffer.c_str();
        }
        case 4: // Music
        {
            NSArray<NSString*>* paths = NSSearchPathForDirectoriesInDomains(NSMusicDirectory, NSUserDomainMask, YES);
            if (paths.count == 0) { return nullptr; }
            buffer = [paths.firstObject UTF8String];
            return buffer.c_str();
        }
        case 5: // Pictures
        {
            NSArray<NSString*>* paths = NSSearchPathForDirectoriesInDomains(NSPicturesDirectory, NSUserDomainMask, YES);
            if (paths.count == 0) { return nullptr; }
            buffer = [paths.firstObject UTF8String];
            return buffer.c_str();
        }
        case 6: // Videos
        {
            NSArray<NSString*>* paths = NSSearchPathForDirectoriesInDomains(NSMoviesDirectory, NSUserDomainMask, YES);
            if (paths.count == 0) { return nullptr; }
            buffer = [paths.firstObject UTF8String];
            return buffer.c_str();
        }
        case 7: // AppDataLocal
        case 8: // AppDataRoaming
        {
            NSArray<NSString*>* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
            if (paths.count == 0) { return nullptr; }
            buffer = [paths.firstObject UTF8String];
            return buffer.c_str();
        }
        case 9: // Temp
            buffer = [NSTemporaryDirectory() UTF8String];
            return buffer.c_str();
        default:
            return nullptr;
    }
}

#endif



#if defined(ARK_PLATFORM_MACOS)

namespace Ark::System
{
    Storage::Path getKnownFolderPath(KnownFolder folder)
    {
        int code = 0;
        switch (folder)
        {
            case KnownFolder::Home: code = 0; break;
            case KnownFolder::Desktop: code = 1; break;
            case KnownFolder::Documents: code = 2; break;
            case KnownFolder::Downloads: code = 3; break;
            case KnownFolder::Music: code = 4; break;
            case KnownFolder::Pictures: code = 5; break;
            case KnownFolder::Videos: code = 6; break;
            case KnownFolder::AppDataLocal: code = 7; break;
            case KnownFolder::AppDataRoaming: code = 8; break;
            case KnownFolder::Temp: code = 9; break;
        }

        char const* path = arkMacosGetKnownFolderPath(code);
        if (path == nullptr)
        {
            return Storage::Path();
        }
        return Storage::Path(String(path));
    }
}

#endif

