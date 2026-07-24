ark_add_target(
    NAME Ark.Cryptography
    ALIAS Ark::Cryptography
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Cryptography"
    SOURCES
    LIBRARIES
        Ark::Core
        Ark::Collections
        Ark::Strings
)
