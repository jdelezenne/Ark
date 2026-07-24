include(FetchContent)

if(ARK_OPTION_JSON_BACKEND STREQUAL "ark" AND ARK_OPTION_XML_BACKEND STREQUAL "ark")
    ark_message(STATUS "Using Ark JSON/XML backends; skipping JSON/XML vendor dependencies.")
endif()

if(ARK_OPTION_EXTERNAL_DEPENDENCIES STREQUAL "fetch")
    if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        include(External/emscripten)
    endif()

    # Fetch zlib/bzip2 for native and Emscripten (Emscripten can link the static libs).
    include(External/zlib)
    include(External/bzip2)

    if(NOT ARK_OPTION_STANDARD_LIBRARY STREQUAL "cpp")
        include(External/fmt)
    endif()

    if(ARK_OPTION_XML_BACKEND STREQUAL "tinyxml2")
        include(External/tinyxml2)
    endif()

    if(ARK_OPTION_JSON_BACKEND STREQUAL "rapidjson")
        include(External/RapidJSON)
    elseif(ARK_OPTION_JSON_BACKEND STREQUAL "nlohmann")
        include(External/NlohmannJson)
    endif()
elseif(ARK_OPTION_EXTERNAL_DEPENDENCIES STREQUAL "system")
    if(NOT ARK_OPTION_STANDARD_LIBRARY STREQUAL "cpp")
        find_package(fmt CONFIG REQUIRED)
        if(TARGET fmt::fmt AND NOT TARGET fmt)
            add_library(fmt ALIAS fmt::fmt)
        endif()
    endif()

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        find_package(ZLIB REQUIRED)
        if(TARGET ZLIB::ZLIB AND NOT TARGET zlib::zlib)
            add_library(zlib::zlib ALIAS ZLIB::ZLIB)
        endif()

        find_package(BZip2 REQUIRED)
        if(TARGET BZip2::BZip2 AND NOT TARGET bzip2::bzip2)
            add_library(bzip2::bzip2 ALIAS BZip2::BZip2)
        elseif(NOT TARGET bzip2::bzip2)
            add_library(bzip2::bzip2 INTERFACE IMPORTED)
            set_target_properties(
                bzip2::bzip2
                PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${BZIP2_INCLUDE_DIRS}"
                    INTERFACE_LINK_LIBRARIES "${BZIP2_LIBRARIES}"
            )
        endif()
    endif()

    if(ARK_OPTION_XML_BACKEND STREQUAL "tinyxml2")
        find_package(tinyxml2 CONFIG REQUIRED)
        if(TARGET tinyxml2::tinyxml2 AND NOT TARGET tinyxml2)
            add_library(tinyxml2 ALIAS tinyxml2::tinyxml2)
        elseif(NOT TARGET tinyxml2)
            message(FATAL_ERROR "tinyxml2 target was not found. Expected tinyxml2::tinyxml2 or tinyxml2.")
        endif()
    endif()

    if(ARK_OPTION_JSON_BACKEND STREQUAL "rapidjson")
        find_package(RapidJSON REQUIRED)
        if(TARGET RapidJSON::RapidJSON AND NOT TARGET rapidjson)
            add_library(rapidjson ALIAS RapidJSON::RapidJSON)
        elseif(NOT TARGET rapidjson)
            add_library(rapidjson INTERFACE)
            target_include_directories(rapidjson INTERFACE ${RapidJSON_INCLUDE_DIRS})
        endif()
    elseif(ARK_OPTION_JSON_BACKEND STREQUAL "nlohmann")
        find_package(nlohmann_json CONFIG QUIET)
        if(TARGET nlohmann_json::nlohmann_json AND NOT TARGET nlohmann_json)
            add_library(nlohmann_json ALIAS nlohmann_json::nlohmann_json)
        elseif(NOT TARGET nlohmann_json)
            find_path(ARK_NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp)
            if(NOT ARK_NLOHMANN_JSON_INCLUDE_DIR)
                message(FATAL_ERROR "nlohmann_json was not found. Install nlohmann_json or provide nlohmann/json.hpp in the include path.")
            endif()

            add_library(nlohmann_json INTERFACE)
            target_include_directories(nlohmann_json INTERFACE ${ARK_NLOHMANN_JSON_INCLUDE_DIR})
            target_compile_definitions(
                nlohmann_json
                INTERFACE
                JSON_NOEXCEPTION
            )
        endif()
    endif()
endif()
