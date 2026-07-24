#pragma once

#include "Ark/Collections/Array.hpp"
#include "Ark/Core/Result.hpp"
#include "Ark/Core/Types.hpp"
#include "Ark/Storage/Path.hpp"
#include "Ark/Strings/String.hpp"
#include "Ark/Strings/StringSlice.hpp"

namespace Ark::System
{
    using ProcessId = uint64;

    /// Launch a process with arguments.
    /// Returns the new process ID on success.
    Result<ProcessId, String> launchProcess(StringSlice path, Collections::Array<String> const& arguments);

    /// Wait for a process to exit. Returns exit code.
    /// timeoutMs: milliseconds to wait; use max uint32 for infinite wait.
    Result<int, String> waitProcess(ProcessId pid, uint32 timeoutMs);

    /// Terminate a process if running.
    Result<Void, String> terminateProcess(ProcessId pid);

    /// Check if a process is still running.
    Result<bool, String> isProcessRunning(ProcessId pid);

    /// Get the full path of the current process executable.
    Storage::Path getCurrentProcessPath();

    /// Get the current working directory of the process.
    Storage::Path getCurrentWorkingDirectory();

    /// Set the current working directory of the process.
    Result<Void, String> setCurrentWorkingDirectory(Storage::Path const& path);

    /// Get the current process ID.
    ProcessId getCurrentProcessId();

    /// Reveal a file or directory in the system file manager (Finder on macOS, Explorer on Windows, file manager on Linux)
    /// @param path The path to reveal
    /// @return Outcome indicating success or failure
    Outcome revealPathInFileManager(Storage::Path const& path);

    /// Open a file with its associated default application
    /// @param path The path to the file to open
    /// @return Outcome indicating success or failure
    Outcome openFileWithDefaultApplication(Storage::Path const& path);
}
