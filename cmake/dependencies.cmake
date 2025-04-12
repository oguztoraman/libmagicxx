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
        --disable-lzlib --enable-year2038
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

add_custom_target(generate_default_database_files
    COMMAND
        make magic.mgc
    COMMAND
        bash -c "for file in Magdir/*; do cat $file >> magic; done"
    COMMAND
        mkdir -p ${magicxx_DEFAULT_DATABASE_PATH}
    COMMAND
        cp magic ${magicxx_DEFAULT_DATABASE_FILE} && rm magic
    COMMAND
        cp magic.mgc ${magicxx_DEFAULT_COMPILED_DATABASE_FILE}
    COMMAND
        echo "Default database files have been generated in ${magicxx_DEFAULT_DATABASE_PATH}."
    WORKING_DIRECTORY
        ${magic_DIR}/magic
    COMMENT
        "Generating default database files..."
    VERBATIM
)

add_dependencies(generate_default_database_files configure_file)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_dependencies(configure_file configure_gnurx)
endif()
