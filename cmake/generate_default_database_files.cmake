# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Generate default database files target
# -----------------------------------------------------------------------------
add_custom_target(generate_default_database_files
    COMMAND
        autoreconf -f -i
    COMMAND
        ./configure --enable-static=yes --enable-shared=yes
        --disable-zlib --disable-bzlib
        --disable-xzlib --disable-zstdlib
        --disable-lzlib --disable-lrziplib
        --enable-year2038
        --host=x86_64-pc-linux-gnu --silent
        CC=gcc
    COMMAND
        make -j
    COMMAND
        bash -c "for file in magic/Magdir/*; do cat $file >> ${magic_DEFAULT_DATABASE_FILE}; done"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${magicxx_DEFAULT_DATABASE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy
        ${magic_DEFAULT_DATABASE_FILE} ${magicxx_DEFAULT_DATABASE_FILE}
    COMMAND ${CMAKE_COMMAND} -E rm
        ${magic_DEFAULT_DATABASE_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy
        ${magic_DEFAULT_COMPILED_DATABASE_FILE} ${magicxx_DEFAULT_COMPILED_DATABASE_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo
        "Default database files have been generated in ${magicxx_DEFAULT_DATABASE_DIR}."
    WORKING_DIRECTORY
        ${magic_DIR}
    COMMENT
        "Generating default database files..."
    VERBATIM
)
