ark_add_target(
    NAME Ark.Logging
    ALIAS Ark::Logging
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Logging"
    SOURCES
    LIBRARIES
        Ark::Core
        Ark::Collections
        Ark::Strings
        Ark::Memory
        Ark::Concurrency
        Ark::System
        Ark::Storage
)
