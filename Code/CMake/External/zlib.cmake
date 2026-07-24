FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG v1.3.1
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(zlib)

set(ZLIB_TARGETS
    zlib
    zlibstatic
)

foreach(_target ${ZLIB_TARGETS})
    if(TARGET ${_target})
        set_target_properties(${_target} PROPERTIES FOLDER "Externals/zlib")
    endif()
endforeach()

if(TARGET zlibstatic)
    add_library(
        zlib::zlib
        ALIAS
        zlibstatic
    )
elseif(TARGET zlib)
    add_library(
        zlib::zlib
        ALIAS
        zlib
    )
endif()
