# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

set(magic_DEFAULT_DATABASE_PATH
    ${magic_DIR}/magic
)

set(magic_DEFAULT_DATABASE_FILE
    ${magic_DEFAULT_DATABASE_PATH}/magic
)

set(magic_DEFAULT_COMPILED_DATABASE_FILE
    ${magic_DEFAULT_DATABASE_PATH}/magic.mgc
)

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
    COMMAND
        mkdir -p ${magicxx_DEFAULT_DATABASE_PATH}
    COMMAND
        cp ${magic_DEFAULT_DATABASE_FILE} ${magicxx_DEFAULT_DATABASE_FILE}
    COMMAND
        rm ${magic_DEFAULT_DATABASE_FILE}
    COMMAND
        cp ${magic_DEFAULT_COMPILED_DATABASE_FILE} ${magicxx_DEFAULT_COMPILED_DATABASE_FILE}
    COMMAND
        echo "Default database files have been generated in ${magicxx_DEFAULT_DATABASE_PATH}."
    WORKING_DIRECTORY
        ${magic_DIR}
    COMMENT
        "Generating default database files..."
    VERBATIM
)
