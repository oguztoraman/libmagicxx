# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Find clang-format
# -----------------------------------------------------------------------------
find_program(CLANG_FORMAT clang-format)

if(NOT CLANG_FORMAT)
    message(FATAL_ERROR "clang-format not found!")
endif()

# -----------------------------------------------------------------------------
# Format source code target
# -----------------------------------------------------------------------------
add_custom_target(format_source_code
    COMMAND
        ${CLANG_FORMAT}
        -i
        -style=file
        ${magicxx_SOURCE_FILES_TO_LINT}
    WORKING_DIRECTORY
        ${magicxx_SOURCE_DIR}
    COMMENT
        "Formatting source code..."
    VERBATIM
)
