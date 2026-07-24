#if defined(ARK_PLATFORM_WINDOWS)

#include "Ark/System/Process.hpp"
#include "Ark/Strings/Unicode.hpp"

#include <Psapi.h>
#include <Windows.h>
#include <shellapi.h>

namespace Ark::System
{
    namespace
    {
        static void appendQuotedWindowsArg(String& cmd, StringSlice arg)
        {
            const char* p = arg.asPointer();
            usize n = arg.getCount();

            bool needs_quotes = (n == 0);
            for (usize i = 0; i < n && !needs_quotes; ++i)
            {
                char c = p[i];
                if (c == ' ' || c == '\t' || c == '\n' || c == '"')
                {
                    needs_quotes = true;
                }
            }

            if (!needs_quotes)
            {
                cmd.append(arg);
                return;
            }

            cmd.push('"');
            usize backslashes = 0;
            for (usize i = 0; i < n; ++i)
            {
                char c = p[i];
                if (c == '\\')
                {
                    ++backslashes;
                }
                else if (c == '"')
                {
                    for (usize k = 0; k < backslashes * 2; ++k)
                    {
                        cmd.push('\\');
                    }
                    backslashes = 0;
                    cmd.push('\\');
                    cmd.push('"');
                }
                else
                {
                    for (usize k = 0; k < backslashes; ++k)
                    {
                        cmd.push('\\');
                    }
                    backslashes = 0;
                    cmd.push(c);
                }
            }
            for (usize k = 0; k < backslashes * 2; ++k)
            {
                cmd.push('\\');
            }
            cmd.push('"');
        }
    }

    Result<ProcessId, String> launchProcess(StringSlice path, Collections::Array<String> const& arguments)
    {
        auto widePath = Unicode::toWide(path);

        String cmd;
        appendQuotedWindowsArg(cmd, path);
        for (usize i = 0; i < arguments.getCount(); ++i)
        {
            cmd.push(' ');
            appendQuotedWindowsArg(cmd, arguments[i]);
        }

        auto wideCmd = Unicode::toWide(cmd);

        STARTUPINFOW si{};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi{};

        BOOL ok = CreateProcessW(widePath.asPointer(), wideCmd.asMutablePointer(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
        if (!ok)
        {
            return Result<ProcessId, String>(unexpectedResult, String::format("CreateProcessW failed: {}", static_cast<int>(GetLastError())));
        }

        CloseHandle(pi.hThread);
        ProcessId pid = static_cast<ProcessId>(pi.dwProcessId);
        CloseHandle(pi.hProcess);
        return pid;
    }

    Result<int, String> waitProcess(ProcessId pid, uint32 timeoutMs)
    {
        HANDLE h = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, static_cast<DWORD>(pid));
        if (h == nullptr)
        {
            return Result<int, String>(unexpectedResult, String("OpenProcess failed"));
        }

        DWORD res = WaitForSingleObject(h, timeoutMs == static_cast<uint32>(-1) ? INFINITE : timeoutMs);
        if (res == WAIT_TIMEOUT)
        {
            CloseHandle(h);
            return Result<int, String>(unexpectedResult, String("Timeout"));
        }
        if (res == WAIT_FAILED)
        {
            CloseHandle(h);
            return Result<int, String>(unexpectedResult, String("WaitForSingleObject failed"));
        }

        DWORD exitCode = 0;
        if (!GetExitCodeProcess(h, &exitCode))
        {
            CloseHandle(h);
            return Result<int, String>(unexpectedResult, String("GetExitCodeProcess failed"));
        }
        CloseHandle(h);
        return static_cast<int>(exitCode);
    }

    Result<Void, String> terminateProcess(ProcessId pid)
    {
        HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<DWORD>(pid));
        if (h == nullptr)
        {
            return Result<Void, String>(unexpectedResult, String("OpenProcess failed"));
        }
        BOOL ok = TerminateProcess(h, 1);
        CloseHandle(h);
        if (!ok)
        {
            return Result<Void, String>(unexpectedResult, String("TerminateProcess failed"));
        }
        return Result<Void, String>();
    }

    Result<bool, String> isProcessRunning(ProcessId pid)
    {
        HANDLE h = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, static_cast<DWORD>(pid));
        if (h == nullptr)
        {
            return Result<bool, String>(unexpectedResult, String("OpenProcess failed"));
        }
        DWORD exitCode = 0;
        BOOL ok = GetExitCodeProcess(h, &exitCode);
        CloseHandle(h);
        if (!ok)
        {
            return Result<bool, String>(unexpectedResult, String("GetExitCodeProcess failed"));
        }
        return exitCode == STILL_ACTIVE;
    }

    Storage::Path getCurrentProcessPath()
    {
        wchar_t buffer[MAX_PATH] = {};
        DWORD len = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        if (len == 0)
        {
            return Storage::Path();
        }
        return Storage::Path(Unicode::fromWide(buffer));
    }

    Storage::Path getCurrentWorkingDirectory()
    {
        DWORD len = GetCurrentDirectoryW(0, nullptr);
        if (len == 0)
        {
            return Storage::Path();
        }
        Ark::Collections::Array<wchar_t> buf;
        buf.resize(len);
        DWORD got = GetCurrentDirectoryW(len, buf.asMutablePointer());
        if (got == 0)
        {
            return Storage::Path();
        }
        return Storage::Path(Unicode::fromWide(buf.asPointer()));
    }

    Result<Void, String> setCurrentWorkingDirectory(Storage::Path const& path)
    {
        auto wide = Unicode::toWide(path.string());
        if (!SetCurrentDirectoryW(wide.asPointer()))
        {
            return Result<Void, String>(unexpectedResult, String("SetCurrentDirectoryW failed"));
        }
        return Result<Void, String>();
    }

    ProcessId getCurrentProcessId()
    {
        return static_cast<ProcessId>(GetCurrentProcessId());
    }

    Outcome revealPathInFileManager(Storage::Path const& path)
    {
        if (path.isEmpty())
        {
            return makeError();
        }

        Collections::Array<String> arguments;
        arguments.append("/select,");
        arguments.append(path.string());
        auto result = launchProcess("explorer.exe", arguments);
        return result.isOk() ? makeOutcome() : makeError();
    }

    Outcome openFileWithDefaultApplication(Storage::Path const& path)
    {
        if (path.isEmpty())
        {
            return makeError();
        }

        // Use ShellExecute to open file with default application
        auto wpath = Unicode::toWide(path.string());
        HINSTANCE result = ShellExecuteW(nullptr, L"open", wpath.asPointer(), nullptr, nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<intptr_t>(result) > 32)
        {
            return makeOutcome();
        }
        return makeError();
    }
}

#endif
