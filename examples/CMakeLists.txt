# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Example applications for Libmagicxx
# Demonstrates usage of the libmagicxx library with both shared and static
# library variants.
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.31.6)

project(magicxx_examples
    DESCRIPTION "Example applications for Libmagicxx."
    LANGUAGES CXX
)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

find_package(Magicxx REQUIRED)

if(NOT MAGICXX_FOUND)
    message(FATAL_ERROR "Libmagicxx not found. Please install it first.")
endif()

message(STATUS "")
message(STATUS "Libmagicxx Configuration:")
message(STATUS "  Version:          ${MAGICXX_VERSION}")
message(STATUS "  Shared library:   ${MAGICXX_SHARED_LIB_AVAILABLE}")
message(STATUS "  Static library:   ${MAGICXX_STATIC_LIB_AVAILABLE}")
message(STATUS "  Include dir:      ${MAGICXX_INCLUDE_DIR}")
message(STATUS "  Library dir:      ${MAGICXX_LIB_DIR}")
message(STATUS "  License dir:      ${MAGICXX_LICENSE_DIR}")
message(STATUS "  Documentation:    ${MAGICXX_DOC_DIR}")
message(STATUS "  CMake dir:        ${MAGICXX_CMAKE_DIR}")
message(STATUS "  Database dir:     ${MAGICXX_DEFAULT_DATABASE_DIR}")
message(STATUS "")

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------

set(magicxx_examples_SOURCE_FILES
    magic_examples.cpp
)

# -----------------------------------------------------------------------------
# Helper function to configure an example executable
# -----------------------------------------------------------------------------
function(configure_example_executable target_name library_target)
    add_executable(${target_name})

    target_sources(${target_name}
        PRIVATE
            ${magicxx_examples_SOURCE_FILES}
    )

    target_compile_features(${target_name}
        PRIVATE
            cxx_std_23
    )

    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
    )

    target_link_libraries(${target_name}
        PRIVATE
            ${library_target}
    )
endfunction()

# -----------------------------------------------------------------------------
# Build example executables
# -----------------------------------------------------------------------------
if(MAGICXX_SHARED_LIB_AVAILABLE)
    configure_example_executable(magicxx_examples Recognition::Magicxx)
endif()

if(MAGICXX_STATIC_LIB_AVAILABLE)
    configure_example_executable(magicxx_examples_static Recognition::MagicxxStatic)
endif()
