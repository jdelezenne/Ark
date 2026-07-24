FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
set(JSON_MultipleHeaders OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(nlohmann_json)

if(TARGET nlohmann_json)
    set_target_properties(nlohmann_json PROPERTIES FOLDER "Externals/nlohmann_json")
endif()

target_compile_definitions(
    nlohmann_json
    INTERFACE
    JSON_NOEXCEPTION
)
