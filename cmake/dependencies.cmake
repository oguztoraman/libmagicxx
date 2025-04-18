# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

add_custom_target(configure_gnurx
    COMMAND
        ./configure
        --host=${FILE_HOST} --silent
        CC=${CMAKE_C_COMPILER}
    WORKING_DIRECTORY
        ${gnurx_DIR}
    COMMENT
        "Configuring the gnurx..."
    VERBATIM
)

add_custom_target(configure_file
    COMMAND
        autoreconf -f -i
    COMMAND
        ./configure --enable-static=yes --enable-shared=yes
        --disable-zlib --disable-bzlib
        --disable-xzlib --disable-zstdlib
        --disable-lzlib --disable-lrziplib
        --enable-year2038
        --host=${FILE_HOST} --silent
        CC=${CMAKE_C_COMPILER}
    COMMAND
        make -C ${magic_SOURCE_DIR} magic.h
    WORKING_DIRECTORY
        ${magic_DIR}
    COMMENT
        "Configuring the file..."
    VERBATIM
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_dependencies(configure_file configure_gnurx)
endif()
