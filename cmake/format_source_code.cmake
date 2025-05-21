# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

find_program(CLANG_FORMAT clang-format)

if (NOT CLANG_FORMAT)
    message(FATAL_ERROR "clang-format not found!")
endif()

add_custom_target(format_source_code
    COMMAND
        clang-format -i -style=file ${magicxx_SOURCE_FILES_TO_FORMAT}
    WORKING_DIRECTORY
        ${magicxx_SOURCE_DIR}
    COMMENT
        "Formatting source code..."
    VERBATIM
)
