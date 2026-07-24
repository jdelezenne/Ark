ark_add_target(
    NAME Ark.Compression
    ALIAS Ark::Compression
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Compression"
    SOURCES
    LIBRARIES
        Ark::Core
        zlib::zlib
        bzip2::bzip2
)
