# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

set(CPACK_SOURCE_GENERATOR
    ZIP
)

set(CPACK_SOURCE_PACKAGE_FILE_NAME
    ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-source
)

set(CPACK_SOURCE_IGNORE_FILES
    "/.git/"
    "/.cache/"
    "/build/"
    "/packages/"
)

add_custom_target(generate_source_package
    COMMAND
        cpack --config build/CPackSourceConfig.cmake
    WORKING_DIRECTORY
        ${magicxx_SOURCE_DIR}
    COMMENT
        "Generating source package..."
    VERBATIM
)
