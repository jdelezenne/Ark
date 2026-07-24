#if defined(ARK_PLATFORM_MACOS) || defined(ARK_PLATFORM_LINUX)

#include "Ark/System/Process.hpp"

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#if defined(ARK_PLATFORM_MACOS)
#include <mach-o/dyld.h>
#endif

extern char** environ;

namespace Ark::System
{
    Result<ProcessId, String> launchProcess(StringSlice path, Collections::Array<String> const& arguments)
    {
        // Build argv
        usize argc = arguments.getCount() + 2; // program + args + null
        Collections::Array<char*> argv;
        argv.resize(argc);
        argv[0] = const_cast<char*>(path.asPointer());
        for (usize i = 0; i < arguments.getCount(); ++i)
        {
            argv[i + 1] = const_cast<char*>(arguments[i].asPointer());
        }
        argv[argc - 1] = nullptr;

        pid_t pid;
        int rc = posix_spawnp(&pid, path.asPointer(), nullptr, nullptr, argv.asPointer(), environ);
        if (rc != 0)
        {
            return Result<ProcessId, String>(unexpectedResult, String("posix_spawn failed"));
        }
        return static_cast<ProcessId>(pid);
    }

    Result<int, String> waitProcess(ProcessId pid, uint32 timeoutMs)
    {
        pid_t wp;
        int status = 0;

        if (timeoutMs == static_cast<uint32>(-1))
        {
            wp = waitpid(static_cast<pid_t>(pid), &status, 0);
        }
        else
        {
            // Polling wait with usleep; simple fallback
            uint32 waited = 0;
            do
            {
                wp = waitpid(static_cast<pid_t>(pid), &status, WNOHANG);
                if (wp == 0)
                {
                    usleep(1000);
                    waited += 1;
                }
            }
            while (wp == 0 && waited < timeoutMs);
        }
        if (wp == 0)
        {
            return Result<int, String>(unexpectedResult, String("Timeout"));
        }
        if (wp < 0)
        {
            return Result<int, String>(unexpectedResult, String("waitpid failed"));
        }

        if (WIFEXITED(status))
        {
            return static_cast<int>(WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            return -static_cast<int>(WTERMSIG(status));
        }
        return 0;
    }

    Result<Void, String> terminateProcess(ProcessId pid)
    {
        if (kill(static_cast<pid_t>(pid), SIGTERM) != 0)
        {
            return Result<Void, String>(unexpectedResult, String("kill(SIGTERM) failed"));
        }
        return Result<Void, String>();
    }

    Result<bool, String> isProcessRunning(ProcessId pid)
    {
        int res = kill(static_cast<pid_t>(pid), 0);
        if (res == 0)
        {
            return true;
        }
        return false;
    }

    Storage::Path getCurrentProcessPath()
    {
        char path[4096] = {};
#if defined(ARK_PLATFORM_LINUX)
        ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
        if (len > 0)
        {
            path[len] = '\0';
            return Storage::Path(String(path));
        }
#elif defined(ARK_PLATFORM_MACOS)
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0)
        {
            return Storage::Path(String(path));
        }
#endif
        return Storage::Path();
    }

    Storage::Path getCurrentWorkingDirectory()
    {
        char buffer[PATH_MAX] = {};
        errno = 0;
        if (getcwd(buffer, sizeof(buffer)) == nullptr)
        {
            return Storage::Path();
        }
        return Storage::Path(String(buffer));
    }

    Result<Void, String> setCurrentWorkingDirectory(Storage::Path const& path)
    {
        if (chdir(path.string().asPointer()) != 0)
        {
            return Result<Void, String>(unexpectedResult, String("chdir failed"));
        }
        return Result<Void, String>();
    }

    ProcessId getCurrentProcessId()
    {
        return static_cast<ProcessId>(getpid());
    }

    Outcome revealPathInFileManager(Storage::Path const& path)
    {
        if (path.isEmpty())
        {
            return makeError();
        }

        Collections::Array<String> arguments;
#if defined(ARK_PLATFORM_MACOS)
        arguments.append("-R");
        arguments.append(path.string());
        auto result = launchProcess("/usr/bin/open", arguments);
#else
        arguments.append(path.parent().string());
        auto result = launchProcess("xdg-open", arguments);
#endif
        return result.isOk() ? makeOutcome() : makeError();
    }

    Outcome openFileWithDefaultApplication(Storage::Path const& path)
    {
        if (path.isEmpty())
        {
            return makeError();
        }

        Collections::Array<String> arguments;
        arguments.append(path.string());
#if defined(ARK_PLATFORM_MACOS)
        auto result = launchProcess("/usr/bin/open", arguments);
#else
        auto result = launchProcess("xdg-open", arguments);
#endif
        return result.isOk() ? makeOutcome() : makeError();
    }
}

#endif
