FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG master
)

set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(rapidjson)

if(NOT TARGET rapidjson)
    add_library(rapidjson INTERFACE)

    target_include_directories(
        rapidjson
        INTERFACE
        ${rapidjson_SOURCE_DIR}/include
    )
endif()

if(TARGET rapidjson)
    set_target_properties(rapidjson PROPERTIES FOLDER "Externals/RapidJSON")
endif()
