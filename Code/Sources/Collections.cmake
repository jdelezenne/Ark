ark_add_target(
    NAME Ark.Collections
    ALIAS Ark::Collections
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Collections"
    SOURCES
    LIBRARIES
        Ark::Core
        Ark::Memory
)
