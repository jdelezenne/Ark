# Download and build official bzip2 source
# Official source: https://sourceware.org/pub/bzip2/

set(BZIP2_VERSION "1.0.8")
set(BZIP2_SHA256 "ab5a03176ee106d3f0fa90e381da478ddae405918153cca248e682cd0c4a2269")
set(BZIP2_URL "https://sourceware.org/pub/bzip2/bzip2-${BZIP2_VERSION}.tar.gz")
set(BZIP2_ROOT_DIR "${CMAKE_BINARY_DIR}/_deps/bzip2")
set(BZIP2_SOURCE_DIR "${BZIP2_ROOT_DIR}/bzip2-${BZIP2_VERSION}")
set(BZIP2_ARCHIVE "${BZIP2_ROOT_DIR}/bzip2-${BZIP2_VERSION}.tar.gz")

# Download the archive if it doesn't exist
if(NOT EXISTS "${BZIP2_ARCHIVE}")
    message(STATUS "Downloading bzip2 ${BZIP2_VERSION}...")
    file(DOWNLOAD
        ${BZIP2_URL}
        ${BZIP2_ARCHIVE}
        EXPECTED_HASH SHA256=${BZIP2_SHA256}
        SHOW_PROGRESS
    )
endif()

# Extract the archive if source doesn't exist
if(NOT EXISTS "${BZIP2_SOURCE_DIR}")
    message(STATUS "Extracting bzip2 ${BZIP2_VERSION}...")
    file(MAKE_DIRECTORY "${BZIP2_SOURCE_DIR}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf ${BZIP2_ARCHIVE}
        WORKING_DIRECTORY ${BZIP2_ROOT_DIR}
        RESULT_VARIABLE extract_result
    )
    if(NOT extract_result EQUAL "0")
        message(FATAL_ERROR "Failed to extract bzip2 archive")
    endif()
endif()

# Check if target already exists
if(NOT TARGET bzip2)
    # Create the library target by compiling the sources directly
    add_library(bzip2 STATIC
        ${BZIP2_SOURCE_DIR}/blocksort.c
        ${BZIP2_SOURCE_DIR}/huffman.c
        ${BZIP2_SOURCE_DIR}/crctable.c
        ${BZIP2_SOURCE_DIR}/randtable.c
        ${BZIP2_SOURCE_DIR}/compress.c
        ${BZIP2_SOURCE_DIR}/decompress.c
        ${BZIP2_SOURCE_DIR}/bzlib.c
    )
    
    # Set include directory
    target_include_directories(bzip2
        PUBLIC
            $<BUILD_INTERFACE:${BZIP2_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include>
    )
    
    # Add compile definitions (needed for large file support on some platforms)
    target_compile_definitions(bzip2 PRIVATE _FILE_OFFSET_BITS=64)
    
    # Disable warnings for third-party code
    if(MSVC)
        target_compile_options(bzip2 PRIVATE /W0)
    else()
        target_compile_options(bzip2 PRIVATE -w)
    endif()
    
    # Set output folder
    set_target_properties(bzip2 PROPERTIES FOLDER "Externals/bzip2")
    
    # Create alias
    add_library(bzip2::bzip2 ALIAS bzip2)
endif()
