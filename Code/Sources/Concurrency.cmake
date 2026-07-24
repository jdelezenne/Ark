ark_add_target(
    NAME Ark.Concurrency
    ALIAS Ark::Concurrency
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Concurrency"
    SOURCES
        ThreadPool.cpp
    LIBRARIES
        Ark::System
)

if (ARK_PLATFORM_WINDOWS)
    target_sources(
        Ark.Concurrency
        PRIVATE
            Ark/Concurrency/Platform/Windows/ConditionVariable.cpp
    )
endif()