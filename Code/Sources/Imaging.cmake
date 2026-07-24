ark_add_target(
    NAME Ark.Imaging
    ALIAS Ark::Imaging
    GROUP Ark/Modules
    TYPE INTERFACE
    DIRECTORY "Ark/Imaging"
    SOURCES
    LIBRARIES
        Ark::Core
        Ark::Collections
        Ark::Math
)
