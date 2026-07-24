ark_add_target(
    NAME Ark.Strings
    ALIAS Ark::Strings
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Strings"
    SOURCES
    LIBRARIES
        Ark::Core
        Ark::Collections
        Ark::Memory
)

if(NOT ARK_OPTION_STANDARD_LIBRARY STREQUAL "cpp")
    target_link_libraries(
        Ark.Strings
        PUBLIC
        fmt::fmt
    )
endif()

if (ARK_PLATFORM_WINDOWS)
    target_sources(
        Ark.Strings
        PRIVATE
        Ark/Strings/Platform/Windows/Unicode.cpp
    )
else()
    target_sources(
        Ark.Strings
        PRIVATE
        Ark/Strings/Platform/CppStd/Unicode.cpp
    )
endif()
