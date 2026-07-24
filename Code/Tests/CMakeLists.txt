set(ARK_TEST_MODULES
    Core
    Memory
    Concurrency
    Collections
    Strings
    System
    Storage
    Documents
    Math
    Logging
    Display
    Application
)

foreach(MODULE ${ARK_TEST_MODULES})
    set(TARGET_NAME "ArkTests${MODULE}")

    file(GLOB_RECURSE MODULE_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.hpp"
    )

    if(MODULE_TEST_SOURCES)
        add_executable(${TARGET_NAME}
            ${MODULE_TEST_SOURCES}
        )

        target_link_libraries(${TARGET_NAME}
            PRIVATE
                Ark::Core
                Ark::Memory
                Ark::Concurrency
                Ark::Collections
                Ark::Strings
                Ark::System
                Ark::Storage
                Ark::Documents
                Ark::Math
                Ark::Logging
                Ark::Display
                Ark::Application
                Catch2::Catch2WithMain
        )

        set_target_properties(${TARGET_NAME} PROPERTIES
            FOLDER "Ark/Tests"
        )

        target_include_directories(${TARGET_NAME}
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}
        )

        target_compile_definitions(${TARGET_NAME}
            PRIVATE
                ARK_TEST_MODULE=${MODULE}
        )

        include(Catch)
        catch_discover_tests(${TARGET_NAME})
    endif()
endforeach()
