FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.11.0
)

FetchContent_MakeAvailable(Catch2)

if(DEFINED Catch2_SOURCE_DIR AND EXISTS "${Catch2_SOURCE_DIR}/extras/Catch.cmake")
    list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
endif()

if(TARGET Catch2::Catch2WithMain)
    set_target_properties(
        Catch2
        PROPERTIES
        FOLDER
        "Externals/Catch2"
    )

    set_target_properties(
        Catch2WithMain
        PROPERTIES
        FOLDER
        "Externals/Catch2"
    )
endif()
