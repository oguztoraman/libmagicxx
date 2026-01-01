# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

add_custom_target(configure_gnurx
    COMMAND
        CC=${CMAKE_C_COMPILER}
        ./configure
        "CFLAGS=-std=c99 ${GNURX_CFLAGS}"
        --host=${GNURX_TARGET}
        --silent
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
        CC=${CMAKE_C_COMPILER}
        ./configure
        "CFLAGS=-std=c99 ${FILE_CFLAGS}"
        --enable-static=yes
        --enable-shared=yes
        --disable-zlib
        --disable-bzlib
        --disable-xzlib
        --disable-zstdlib
        --disable-lzlib
        --disable-lrziplib
        --enable-year2038
        --host=${FILE_TARGET}
        --silent
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
