cmake_minimum_required(VERSION 3.16)
project(RingHTML LANGUAGES C)

include(FetchContent)

set(RING_ROOT_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
if(DEFINED ENV{RING})
    set(RING_ROOT_DEFAULT "$ENV{RING}")
endif()
set(RING_ROOT "${RING_ROOT_DEFAULT}" CACHE PATH "Path to the Ring project root directory.")
message(STATUS "Using Ring from: ${RING_ROOT}")

if(NOT TARGET Ring::Ring)
    add_library(Ring::Ring UNKNOWN IMPORTED)
    find_path(RING_INCLUDE_DIR ring.h PATHS "${RING_ROOT}/language/include")
    find_library(RING_LIBRARY_PATH NAMES ring PATHS "${RING_ROOT}/lib")

    if(NOT RING_INCLUDE_DIR OR NOT RING_LIBRARY_PATH)
        message(FATAL_ERROR "Could not find Ring headers or library in ${RING_ROOT}. "
                            "Please set the RING_ROOT cache variable correctly.")
    endif()

    set_target_properties(Ring::Ring PROPERTIES
        IMPORTED_LOCATION "${RING_LIBRARY_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${RING_INCLUDE_DIR}"
    )
endif()

FetchContent_Declare(
    lexbor
    GIT_REPOSITORY https://github.com/lexbor/lexbor.git
    GIT_TAG v2.7.0
)

set(LEXBOR_BUILD_SHARED OFF CACHE BOOL "Build lexbor as a shared library")
set(LEXBOR_BUILD_STATIC ON CACHE BOOL "Disable building static library for lexbor")
set(LEXBOR_BUILD_TESTS OFF CACHE BOOL "Disable building tests for lexbor")
set(LEXBOR_BUILD_EXAMPLES OFF CACHE BOOL "Disable building examples for lexbor")
set(LEXBOR_BUILD_BENCHMARKS OFF CACHE BOOL "Disable building benchmarks for lexbor")
set(LEXBOR_BUILD_UTILS OFF CACHE BOOL "Disable building utils for lexbor")
set(LEXBOR_INSTALL_HEADERS OFF CACHE BOOL "Disable installing headers for lexbor")

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/c_src/lexbor")
    message(STATUS "lexbor not found locally, fetching...")
    FetchContent_MakeAvailable(lexbor)
else()
    message(STATUS "lexbor found locally, adding as subdirectory.")
    add_subdirectory(src/c_src/lexbor)
endif()

add_library(ring_html SHARED src/c_src/ring_html.c)

target_link_libraries(ring_html PRIVATE
    Ring::Ring
    lexbor_static
)

target_compile_options(ring_html PRIVATE
    $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:
        $<$<C_COMPILER_ID:MSVC>:/O2>
        $<$<NOT:$<C_COMPILER_ID:MSVC>>:-O3>
        -DNDEBUG
    >
)

string(TOLOWER "${CMAKE_SYSTEM_NAME}" OS_DIR)
if(OS_DIR STREQUAL "darwin")
    set(OS_DIR "macos")
endif()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH_DIR_RAW)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND DEFINED CMAKE_GENERATOR_PLATFORM)
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(ARCH_DIR_RAW "i386")
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        set(ARCH_DIR_RAW "amd64")
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
        set(ARCH_DIR_RAW "arm64")
    else()
        message(WARNING "Unsupported CMAKE_GENERATOR_PLATFORM: ${CMAKE_GENERATOR_PLATFORM}, falling back to CMAKE_SYSTEM_PROCESSOR")
    endif()
endif()

if(NOT DEFINED ARCH_DIR)
    if(ARCH_DIR_RAW MATCHES "x86_64|amd64")
        set(ARCH_DIR "amd64")
    elseif(ARCH_DIR_RAW MATCHES "aarch64|arm64")
        set(ARCH_DIR "arm64")
    elseif(ARCH_DIR_RAW MATCHES "riscv64")
        set(ARCH_DIR "riscv64")
    elseif(ARCH_DIR_RAW MATCHES "i386|i686|x8x")
        set(ARCH_DIR "i386")
    else()
        set(ARCH_DIR "${ARCH_DIR_RAW}")
        message(WARNING "Unsupported architecture: ${ARCH_DIR_RAW}. Using as directory name.")
    endif()
endif()

set(LIB_DEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OS_DIR}/${ARCH_DIR}")

add_custom_command(
 	TARGET ring_html
 	POST_BUILD
 	COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_DEST_DIR}
 	COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE:ring_html> ${LIB_DEST_DIR}/$<TARGET_FILE_NAME:ring_html>
 	COMMENT "Moving built library to ${LIB_DEST_DIR}"
 	VERBATIM
 )

install(TARGETS ring_html
    LIBRARY DESTINATION "${RING_ROOT}/lib"
 )

message(STATUS "Ring HTML Extension Configuration:")
message(STATUS "  - Ring Include Dir: ${RING_INCLUDE_DIR}")
message(STATUS "  - Ring Library Path: ${RING_LIBRARY_PATH}")