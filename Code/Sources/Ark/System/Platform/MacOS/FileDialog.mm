#if defined(ARK_PLATFORM_MACOS)

#include "Ark/System/FileDialog.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Storage/Path.hpp"

#import <Cocoa/Cocoa.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

namespace Ark::System
{
    static void applyFilters(NSOpenPanel* panel, FileDialogFilter const* filters, Ark::usize filterCount)
    {
        if (filterCount == 0 || filters == nullptr)
        {
            return;
        }

        NSMutableArray<UTType*>* contentTypes = [NSMutableArray array];
        for (Ark::usize i = 0; i < filterCount; ++i)
        {
            Ark::StringSlice pat = filters[i].pattern;
            if (pat.isEmpty())
            {
                continue;
            }

            Ark::Collections::Array<Ark::StringSlice> parts;
            {
                Ark::StringSlice s = pat;
                Ark::usize start = 0;
                for (Ark::usize k = 0; k <= s.getCount(); ++k)
                {
                    if (k == s.getCount() || s[k] == ';')
                    {
                        Ark::usize len = k - start;
                        if (len > 0)
                        {
                            parts.append(s.subslice(start, k));
                        }
                        start = k + 1;
                    }
                }
            }
            for (Ark::usize j = 0; j < parts.getCount(); ++j)
            {
                NSString* ext = [NSString stringWithUTF8String:parts[j].asPointer()];
                if (ext.length > 0)
                {
                    UTType* t = [UTType typeWithFilenameExtension:ext];
                    if (t != nil)
                    {
                        [contentTypes addObject:t];
                    }
                }
            }
        }

        // Use modern API only; legacy fallbacks are not supported.
        if (contentTypes.count > 0)
        {
            panel.allowedContentTypes = contentTypes;
        }
    }

    Result<Storage::Path, String> showOpenFileDialog(FileDialogOptions const& options)
    {
        @autoreleasepool
        {
            NSOpenPanel* panel = [NSOpenPanel openPanel];
            panel.canChooseFiles = YES;
            panel.canChooseDirectories = NO;
            panel.allowsMultipleSelection = options.allowMultiple ? YES : NO;
            panel.showsHiddenFiles = options.showHidden ? YES : NO;
            if (!options.title.isEmpty()) { panel.title = [NSString stringWithUTF8String:options.title.asPointer()]; }
            if (!options.defaultDirectory.isEmpty()) { panel.directoryURL = [NSURL fileURLWithPath:[NSString stringWithUTF8String:options.defaultDirectory.string().asPointer()]]; }
            applyFilters(panel, options.filters, options.filterCount);
            applyFilters(panel, options.filters, options.filterCount);

            NSModalResponse resp = [panel runModal];
            if (resp == NSModalResponseOK && panel.URL != nil)
            {
                Ark::String path([[panel.URL path] UTF8String]);
                return Ark::Result<Ark::Storage::Path, Ark::String>(Ark::Storage::Path(path));
            }

            return Ark::Result<Ark::Storage::Path, Ark::String>(Ark::unexpectedResult, Ark::String("Canceled"));
        }
    }

    Result<Storage::Path, String> showSaveFileDialog(FileDialogOptions const& options)
    {
        @autoreleasepool
        {
            NSSavePanel* panel = [NSSavePanel savePanel];
            panel.showsHiddenFiles = options.showHidden ? YES : NO;
            if (!options.title.isEmpty()) { panel.title = [NSString stringWithUTF8String:options.title.asPointer()]; }
            if (!options.defaultDirectory.isEmpty()) { panel.directoryURL = [NSURL fileURLWithPath:[NSString stringWithUTF8String:options.defaultDirectory.string().asPointer()]]; }
            if (!options.defaultName.isEmpty()) { panel.nameFieldStringValue = [NSString stringWithUTF8String:options.defaultName.asPointer()]; }

            NSModalResponse resp = [panel runModal];
            if (resp == NSModalResponseOK && panel.URL != nil)
            {
                Ark::String path([[panel.URL path] UTF8String]);
                return Ark::Result<Ark::Storage::Path, Ark::String>(Ark::Storage::Path(path));
            }
            return Ark::Result<Ark::Storage::Path, Ark::String>(Ark::unexpectedResult, Ark::String("Canceled"));
        }
    }

    Result<Storage::Path, String> showSelectFolderDialog(FileDialogOptions const& options)
    {
        @autoreleasepool
        {
            NSOpenPanel* panel = [NSOpenPanel openPanel];
            panel.canChooseFiles = NO;
            panel.canChooseDirectories = YES;
            panel.allowsMultipleSelection = options.allowMultiple ? YES : NO;
            panel.showsHiddenFiles = options.showHidden ? YES : NO;
            if (!options.title.isEmpty()) { panel.title = [NSString stringWithUTF8String:options.title.asPointer()]; }
            if (!options.defaultDirectory.isEmpty()) { panel.directoryURL = [NSURL fileURLWithPath:[NSString stringWithUTF8String:options.defaultDirectory.string().asPointer()]]; }

            NSModalResponse resp = [panel runModal];
            if (resp == NSModalResponseOK && panel.URL != nil)
            {
                Ark::String path([[panel.URL path] UTF8String]);
                return Ark::Result<Ark::Storage::Path, Ark::String>(Ark::Storage::Path(path));
            }
            return Ark::Result<Ark::Storage::Path, Ark::String>(Ark::unexpectedResult, Ark::String("Canceled"));
        }
    }
}

#endif


