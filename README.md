# Ark

Ark is a modular C++23 library.

It provides core runtime building blocks and cross-platform modules for Windows, macOS, Linux, and WebAssembly.

## Overview

- Language standard: C++23
- Platforms: Windows, macOS, Linux, WebAssembly
- Build: CMake presets for configure, build, and test

## Content

- Ark::Core: Option/Result/Error/Variant, traits and concepts, flags, tuples/pairs, platform and type utilities
- Ark::Collections: Array and Slice types, inline arrays/slices, hash maps/sets, ordered maps/sets, collection algorithms
- Ark::Memory: allocator set (linear, growing linear, pool, frame, small object, routing, tracking), virtual memory, unique/shared pointers
- Ark::Strings: UTF-8 and ANSI string APIs, conversions, Base64, UUID, character and encoding helpers
- Ark::Math: vector and matrix types, quaternion, geometry primitives, interpolation, random generators/utilities, color types
- Ark::Concurrency: atomics, mutexes, locks, condition variable, semaphore, thread/thread-local APIs, thread pool
- Ark::Storage: file and filesystem APIs, path utilities, stream interfaces, memory streams, virtual filesystem
- Ark::Documents: INI, CSV, JSON readers/writers, XML backend and document interfaces
- Ark::System: time/clock/timer, process and environment APIs, CPU/system info, clipboard, console, file dialog, message box
- Ark::Display: monitor/window/surface and pixel format abstractions
- Ark::Application: application lifecycle, event loop, event source/dispatcher
- Ark::Compression: gzip, deflate, and bzip2 modules
- Ark::Cryptography: CRC32, MD5, and SHA1
- Ark::Logging: logger/service, log levels/filters, console and file targets, rotating file target, pattern/json/xml formatters
- Ark::Imaging: image types and views, format metadata, color conversion, analysis and processing interfaces

## Quick Start

### 1) Configure

Pick a preset from `CMakePresets.json`.

```bash
# macOS debug
cmake --preset macos-ninja-debug
```

### 2) Build

```bash
cmake --build --preset macos-ninja-debug
```

### 3) Test

```bash
ctest --preset macos-ninja-debug --output-on-failure
```

## Presets

- Windows: `windows-msvc-*`, `windows-ninja-*`, `windows-clang-*`
- macOS: `macos-ninja-*`, `macos-xcode-*`
- Linux: `linux-ninja-*`

Use the same preset name for `cmake --preset`, `cmake --build --preset`, and `ctest --preset`.

## Common CMake Options

Build toggles:

- `ARK_OPTION_BUILD_TESTS`: `ON` or `OFF`
- `ARK_OPTION_BUILD_EXAMPLES`: `ON` or `OFF`
- `ARK_OPTION_BUILD_TRACE`: `ON` or `OFF`

Backend and dependency selection:

- `ARK_OPTION_EXTERNAL_DEPENDENCIES`: `fetch` (default), `system`
- `ARK_OPTION_JSON_BACKEND`: `ark` (default), `nlohmann`, `rapidjson`
- `ARK_OPTION_XML_BACKEND`: `ark` (default), `tinyxml2`
- `ARK_OPTION_STANDARD_LIBRARY`: `generic` (default), `cpp`, `c`

With the defaults (`ark`/`ark`), no JSON/XML vendor dependency is added.

Example:

```bash
cmake --preset macos-ninja-debug \
	-DARK_OPTION_EXTERNAL_DEPENDENCIES=system \
	-DARK_OPTION_BUILD_EXAMPLES=ON
```

## Install And Use In Another Project

Build and install with system dependencies:

```bash
cmake --preset macos-ninja-release -DARK_OPTION_EXTERNAL_DEPENDENCIES=system
cmake --build --preset macos-ninja-release
cmake --install Build/macos-ninja-release --prefix /your/install/prefix
```

Then consume in your own CMake project:

```cmake
find_package(Ark CONFIG REQUIRED)
target_link_libraries(MyApp PRIVATE Ark::Core Ark::Collections Ark::Strings)
```

When `ARK_OPTION_EXTERNAL_DEPENDENCIES=system`, Ark installs package metadata (`ArkConfig.cmake` and exported targets) for `find_package` integration.

## Repository Structure

- [Code/Sources](Code/Sources): module source and public headers
- [Code/Examples](Code/Examples): example programs
- [Code/Tests](Code/Tests): automated tests
- [Code/CMake](Code/CMake): options, dependency wiring, package config
- [CMakePresets.json](CMakePresets.json): configure/build/test presets

## Version Information

Ark versioning is exposed through:

- [CMakeLists.txt](CMakeLists.txt) project version metadata
- generated `ArkVersion.hpp` macros (`ARK_VERSION_MAJOR`, `ARK_VERSION_MINOR`, `ARK_VERSION_PATCH`, `ARK_VERSION`)
