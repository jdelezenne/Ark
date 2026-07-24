ark_add_target(
    NAME Ark.Documents
    ALIAS Ark::Documents
    GROUP Ark/Modules
    TYPE STATIC
    DIRECTORY "Ark/Documents"
    LIBRARIES
        Ark::Core
        Ark::Collections
        Ark::Memory
        Ark::Strings
        Ark::Storage
)

if(ARK_OPTION_JSON_BACKEND STREQUAL "ark")
    target_compile_definitions(
        Ark.Documents
        PUBLIC
            ARK_JSON_BACKEND_ARK
    )
elseif(ARK_OPTION_JSON_BACKEND STREQUAL "rapidjson")
    target_link_libraries(
        Ark.Documents
        PUBLIC
            rapidjson
    )
    target_compile_definitions(
        Ark.Documents
        PUBLIC
            ARK_JSON_BACKEND_RAPIDJSON
    )
elseif(ARK_OPTION_JSON_BACKEND STREQUAL "nlohmann")
    target_link_libraries(
        Ark.Documents
        PUBLIC
            nlohmann_json
    )
    target_compile_definitions(
        Ark.Documents
        PUBLIC
            ARK_JSON_BACKEND_NLOHMANN
    )
endif()

if(ARK_OPTION_XML_BACKEND STREQUAL "ark")
    target_compile_definitions(
        Ark.Documents
        PUBLIC
            ARK_XML_BACKEND_ARK
    )
elseif(ARK_OPTION_XML_BACKEND STREQUAL "tinyxml2")
    target_link_libraries(
        Ark.Documents
        PUBLIC
            tinyxml2
    )
    target_compile_definitions(
        Ark.Documents
        PUBLIC
            ARK_XML_BACKEND_TINYXML2
    )
endif()
