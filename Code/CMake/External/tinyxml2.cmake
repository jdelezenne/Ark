FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 11.0.0
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(tinyxml2_BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(tinyxml2)

if(TARGET tinyxml2)
    set_target_properties(tinyxml2 PROPERTIES FOLDER "Externals/tinyxml2")
endif()


