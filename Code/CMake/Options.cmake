option(ARK_OPTION_BUILD_TRACE "Build with trace support" OFF)
option(ARK_OPTION_BUILD_TESTS "Build the tests" OFF)
option(ARK_OPTION_BUILD_EXAMPLES "Build the examples" OFF)

option(ARK_OPTION_PLATFORM_GDK "Enable GDK platform" OFF)
option(ARK_OPTION_PLATFORM_UWP "Enable UWP platform" OFF)

if(ARK_OPTION_PLATFORM_GDK AND ARK_OPTION_PLATFORM_UWP)
    message(FATAL_ERROR "Cannot enable GDK and UWP together.")
endif()


set(ARK_OPTION_JSON_BACKEND "ark" CACHE STRING "JSON backend to use for Ark (ark, rapidjson, or nlohmann)")
set_property(CACHE ARK_OPTION_JSON_BACKEND PROPERTY STRINGS ark rapidjson nlohmann)
if(NOT ARK_OPTION_JSON_BACKEND STREQUAL "ark" AND NOT ARK_OPTION_JSON_BACKEND STREQUAL "rapidjson" AND NOT ARK_OPTION_JSON_BACKEND STREQUAL "nlohmann")
    message(FATAL_ERROR "ARK_OPTION_JSON_BACKEND must be one of: 'ark', 'rapidjson', or 'nlohmann'.")
endif()

set(ARK_OPTION_XML_BACKEND "ark" CACHE STRING "XML backend to use for Ark (ark or tinyxml2)")
set_property(CACHE ARK_OPTION_XML_BACKEND PROPERTY STRINGS ark tinyxml2)
if(NOT ARK_OPTION_XML_BACKEND STREQUAL "ark" AND NOT ARK_OPTION_XML_BACKEND STREQUAL "tinyxml2")
    message(FATAL_ERROR "ARK_OPTION_XML_BACKEND must be either 'ark' or 'tinyxml2'.")
endif()

set(ARK_OPTION_STANDARD_LIBRARY "generic" CACHE STRING "Standard library mode for Ark (generic, cpp, c)")
set_property(CACHE ARK_OPTION_STANDARD_LIBRARY PROPERTY STRINGS generic cpp c)
if(NOT ARK_OPTION_STANDARD_LIBRARY STREQUAL "generic" AND NOT ARK_OPTION_STANDARD_LIBRARY STREQUAL "cpp" AND NOT ARK_OPTION_STANDARD_LIBRARY STREQUAL "c")
    message(FATAL_ERROR "ARK_OPTION_STANDARD_LIBRARY must be one of: generic, cpp, c.")
endif()

set(ARK_OPTION_EXTERNAL_DEPENDENCIES "fetch" CACHE STRING "Dependency provider for Ark (fetch or system)")
set_property(CACHE ARK_OPTION_EXTERNAL_DEPENDENCIES PROPERTY STRINGS fetch system)
if(NOT ARK_OPTION_EXTERNAL_DEPENDENCIES STREQUAL "fetch" AND NOT ARK_OPTION_EXTERNAL_DEPENDENCIES STREQUAL "system")
    message(FATAL_ERROR "ARK_OPTION_EXTERNAL_DEPENDENCIES must be either 'fetch' or 'system'.")
endif()
