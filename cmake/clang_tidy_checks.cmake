# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Find run-clang-tidy and clang-tidy
# -----------------------------------------------------------------------------
find_program(RUN_CLANG_TIDY run-clang-tidy)

if(NOT RUN_CLANG_TIDY)
    message(FATAL_ERROR "run-clang-tidy not found!")
endif()

find_program(CLANG_TIDY clang-tidy)

if(NOT CLANG_TIDY)
    message(FATAL_ERROR "clang-tidy not found!")
endif()

# -----------------------------------------------------------------------------
# Verify clang-tidy configuration
# -----------------------------------------------------------------------------
add_custom_target(verify_clang_tidy_config
    COMMAND
        ${CLANG_TIDY}
        --config-file=.clang-tidy
        --verify-config
    WORKING_DIRECTORY
        ${magicxx_SOURCE_DIR}
    COMMENT
        "Verifying clang-tidy configuration..."
    VERBATIM
)

# -----------------------------------------------------------------------------
# Run clang-tidy target
# -----------------------------------------------------------------------------
add_custom_target(clang_tidy_checks
    COMMAND
        ${RUN_CLANG_TIDY}
            -clang-tidy-binary ${CLANG_TIDY}
            -p ${CMAKE_BINARY_DIR}
            -j 10
            ${magicxx_SOURCE_FILES_TO_LINT}
    WORKING_DIRECTORY
        ${magicxx_SOURCE_DIR}
    COMMENT
        "Running clang-tidy checks..."
    VERBATIM
)

add_dependencies(clang_tidy_checks verify_clang_tidy_config)
