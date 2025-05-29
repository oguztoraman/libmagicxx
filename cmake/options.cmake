# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

option(INSTALL_MAGICXX
    "Enable installation of Libmagicxx." ON
)

option(BUILD_MAGICXX_SHARED_LIB
    "Build recognition::magicxx shared library." ON
)

option(BUILD_MAGICXX_STATIC_LIB
    "Build recognition::magicxx_static static library." ON
)

option(BUILD_MAGICXX_TESTS
    "Build the tests." OFF
)

option(BUILD_MAGICXX_EXAMPLES
    "Build the examples." OFF
)

option(BUILD_MAGICXX_DOCUMENTATION
    "Build the documentation." OFF
)

message(STATUS "LIBMAGICXX VERSION: ${magicxx_VERSION}")
message(STATUS "BUILD TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "INSTALL_MAGICXX: ${INSTALL_MAGICXX}")
message(STATUS "BUILD_MAGICXX_SHARED_LIB: ${BUILD_MAGICXX_SHARED_LIB}")
message(STATUS "BUILD_MAGICXX_STATIC_LIB: ${BUILD_MAGICXX_STATIC_LIB}")
message(STATUS "BUILD_MAGICXX_TESTS: ${BUILD_MAGICXX_TESTS}")
message(STATUS "BUILD_MAGICXX_EXAMPLES: ${BUILD_MAGICXX_EXAMPLES}")
message(STATUS "BUILD_MAGICXX_DOCUMENTATION: ${BUILD_MAGICXX_DOCUMENTATION}")

if (NOT (BUILD_MAGICXX_EXAMPLES OR BUILD_MAGICXX_SHARED_LIB OR BUILD_MAGICXX_STATIC_LIB))
    message(FATAL_ERROR
        "At least one of the following options must be ON:\n"
        "  + BUILD_MAGICXX_EXAMPLES\n"
        "  + BUILD_MAGICXX_SHARED_LIB\n"
        "  + BUILD_MAGICXX_STATIC_LIB"
    )
endif()
