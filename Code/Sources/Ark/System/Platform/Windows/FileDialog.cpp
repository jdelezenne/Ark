#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/System/FileDialog.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/Unicode.hpp"

#include <commdlg.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <windows.h>

namespace Ark::System
{
    static Result<Storage::Path, String> comOpenFile(FileDialogOptions const& options)
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        bool coInited = SUCCEEDED(hr);
        IFileOpenDialog* pDlg = nullptr;
        Result<Storage::Path, String> result(Ark::unexpectedResult, String("Canceled"));
        if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDlg))))
        {
            DWORD opts = 0;
            if (SUCCEEDED(pDlg->GetOptions(&opts)))
            {
                opts |= FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST;
                if (options.allowMultiple)
                    opts |= FOS_ALLOWMULTISELECT;
                pDlg->SetOptions(opts);
            }

            if (options.title.getLength() > 0)
            {
                auto w = Unicode::toWide(options.title);
                pDlg->SetTitle(w.asPointer());
            }

            if (SUCCEEDED(pDlg->Show(reinterpret_cast<HWND>(options.owner))))
            {
                IShellItem* pItem = nullptr;
                if (SUCCEEDED(pDlg->GetResult(&pItem)) && pItem != nullptr)
                {
                    PWSTR pszFilePath = nullptr;
                    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))
                    {
                        result = Storage::Path(Unicode::fromWide(pszFilePath));
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pDlg->Release();
        }

        if (coInited)
            CoUninitialize();
        return result;
    }

    static Result<Storage::Path, String> comSaveFile(FileDialogOptions const& options)
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        bool coInited = SUCCEEDED(hr);
        IFileSaveDialog* pDlg = nullptr;
        Result<Storage::Path, String> result(Ark::unexpectedResult, String("Canceled"));
        if (SUCCEEDED(CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDlg))))
        {
            DWORD opts = 0;
            if (SUCCEEDED(pDlg->GetOptions(&opts)))
            {
                opts |= FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT;
                pDlg->SetOptions(opts);
            }

            if (options.title.getLength() > 0)
            {
                auto w = Unicode::toWide(options.title);
                pDlg->SetTitle(w.asPointer());
            }

            if (SUCCEEDED(pDlg->Show(reinterpret_cast<HWND>(options.owner))))
            {
                IShellItem* pItem = nullptr;
                if (SUCCEEDED(pDlg->GetResult(&pItem)) && pItem != nullptr)
                {
                    PWSTR pszFilePath = nullptr;
                    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))
                    {
                        result = Storage::Path(Unicode::fromWide(pszFilePath));
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pDlg->Release();
        }

        if (coInited)
            CoUninitialize();
        return result;
    }

    Result<Storage::Path, String> showOpenFileDialog(FileDialogOptions const& options)
    {
        // Try modern COM dialog first
        {
            auto r = comOpenFile(options);
            if (r)
                return r;
        }
        // Fallback: Win32 common dialog
        OPENFILENAMEW ofn = {};
        wchar_t fileBuffer[MAX_PATH] = L"";
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = reinterpret_cast<HWND>(options.owner);
        ofn.lpstrFile = fileBuffer;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        BOOL ok = GetOpenFileNameW(&ofn);
        if (ok)
        {
            return Storage::Path(Unicode::fromWide(ofn.lpstrFile));
        }
        return Result<Storage::Path, String>(Ark::unexpectedResult, String("Canceled"));
    }

    Result<Storage::Path, String> showSaveFileDialog(FileDialogOptions const& options)
    {
        // Try modern COM dialog first
        {
            auto r = comSaveFile(options);
            if (r)
                return r;
        }
        OPENFILENAMEW ofn = {};
        wchar_t fileBuffer[MAX_PATH] = L"";
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = reinterpret_cast<HWND>(options.owner);
        ofn.lpstrFile = fileBuffer;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

        BOOL ok = GetSaveFileNameW(&ofn);
        if (ok)
        {
            return Storage::Path(Unicode::fromWide(ofn.lpstrFile));
        }
        return Result<Storage::Path, String>(Ark::unexpectedResult, String("Canceled"));
    }

    Result<Storage::Path, String> showSelectFolderDialog(FileDialogOptions const& options)
    {
        BROWSEINFOW bi = {};
        bi.hwndOwner = reinterpret_cast<HWND>(options.owner);
        bi.lpszTitle = L"Select Folder";
        PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
        if (pidl)
        {
            wchar_t path[MAX_PATH] = L"";
            if (SHGetPathFromIDListW(pidl, path))
            {
                return Storage::Path(Unicode::fromWide(path));
            }
        }
        return Result<Storage::Path, String>(Ark::unexpectedResult, String("Canceled"));
    }
}

#endif
