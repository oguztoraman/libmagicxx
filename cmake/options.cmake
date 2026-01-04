# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

include(FeatureSummary)

# -----------------------------------------------------------------------------
# Build options
# -----------------------------------------------------------------------------
option(INSTALL_MAGICXX
    "Enable installation of Libmagicxx"
    ON
)

option(BUILD_MAGICXX_SHARED_LIB
    "Build Recognition::Magicxx shared library"
    ON
)

option(BUILD_MAGICXX_STATIC_LIB
    "Build Recognition::MagicxxStatic static library"
    ON
)

option(BUILD_MAGICXX_TESTS
    "Build the unit tests"
    OFF
)

option(BUILD_MAGICXX_EXAMPLES
    "Build the example programs"
    OFF
)

option(BUILD_MAGICXX_DOCUMENTATION
    "Build the Doxygen documentation"
    OFF
)

# -----------------------------------------------------------------------------
# Validation
# -----------------------------------------------------------------------------
if(NOT (BUILD_MAGICXX_EXAMPLES OR BUILD_MAGICXX_SHARED_LIB OR BUILD_MAGICXX_STATIC_LIB))
    message(FATAL_ERROR
        "At least one of the following options must be ON:\n"
        "  - BUILD_MAGICXX_EXAMPLES\n"
        "  - BUILD_MAGICXX_SHARED_LIB\n"
        "  - BUILD_MAGICXX_STATIC_LIB"
    )
endif()

# -----------------------------------------------------------------------------
# Feature summary
# -----------------------------------------------------------------------------
add_feature_info("Shared Library" BUILD_MAGICXX_SHARED_LIB
    "Build the shared library (Recognition::Magicxx)"
)

add_feature_info("Static Library" BUILD_MAGICXX_STATIC_LIB
    "Build the static library (Recognition::MagicxxStatic)"
)

add_feature_info("Unit Tests" BUILD_MAGICXX_TESTS
    "Build unit tests using GoogleTest"
)

add_feature_info("Examples" BUILD_MAGICXX_EXAMPLES
    "Build example programs"
)

add_feature_info("Documentation" BUILD_MAGICXX_DOCUMENTATION
    "Build Doxygen documentation"
)

add_feature_info("Installation" INSTALL_MAGICXX
    "Enable installation targets"
)

# -----------------------------------------------------------------------------
# Status messages
# -----------------------------------------------------------------------------
message(STATUS "")
message(STATUS "Libmagicxx ${magicxx_VERSION} Configuration:")
message(STATUS "  Build type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  C++ Compiler:     ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "")
