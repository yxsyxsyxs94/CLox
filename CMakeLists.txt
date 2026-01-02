cmake_minimum_required(VERSION 3.15)

# Project definition
project(CLox
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "A bytecode virtual machine for the Lox programming language"
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(CLOX_SOURCES
    chunk.cpp
    compiler.cpp
    debug.cpp
    main.cpp
    memory.cpp
    scanner.cpp
    value.cpp
    vm.cpp
)

# Header files
set(CLOX_HEADERS
    chunk.h
    common.h
    compiler.h
    debug.h
    memory.h
    scanner.h
    value.h
    vm.h
)

# Create executable
add_executable(CLox ${CLOX_SOURCES} ${CLOX_HEADERS})

# Compiler-specific options
if(MSVC)
    target_compile_options(CLox PRIVATE
        /W3          # Warning level 3
        /permissive- # Standards conformance mode
        /sdl         # Enable additional security checks
    )
    target_compile_definitions(CLox PRIVATE
        _CONSOLE
        UNICODE
        _UNICODE
    )
    # Set subsystem to console
    set_target_properties(CLox PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
else()
    target_compile_options(CLox PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Platform-specific definitions
if(WIN32)
    target_compile_definitions(CLox PRIVATE
        WIN32
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )
else()
    target_compile_definitions(CLox PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )
endif()

# Configuration-specific settings
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(CLox PRIVATE /O2)
    else()
        target_compile_options(CLox PRIVATE -O3)
    endif()
endif()

# IDE support: organize files in folders
source_group("Source Files" FILES ${CLOX_SOURCES})
source_group("Header Files" FILES ${CLOX_HEADERS})

# Print build information
message(STATUS "Building CLox ${PROJECT_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
