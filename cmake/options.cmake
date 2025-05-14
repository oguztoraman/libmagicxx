# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

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
message(STATUS "BUILD_MAGICXX_TESTS: ${BUILD_MAGICXX_TESTS}")
message(STATUS "BUILD_MAGICXX_EXAMPLES: ${BUILD_MAGICXX_EXAMPLES}")
message(STATUS "BUILD_MAGICXX_DOCUMENTATION: ${BUILD_MAGICXX_DOCUMENTATION}")
