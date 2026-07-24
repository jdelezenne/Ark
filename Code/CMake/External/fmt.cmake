FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.2.0
)

set(FMT_INSTALL OFF CACHE BOOL "" FORCE)
set(FMT_TEST OFF CACHE BOOL "" FORCE)
set(FMT_DOC OFF CACHE BOOL "" FORCE)
set(FMT_EXCEPTIONS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(fmt)

if(TARGET fmt)
    set_target_properties(
        fmt
        PROPERTIES
        FOLDER
        "Externals/fmt"
    )
endif()
