# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

add_custom_target(install_dependencies
    COMMAND
        sudo dnf install -y
            git
            autoconf libtool make
            cmake ninja-build
            g++ clang libcxx-devel mold
            clang-tools-extra doxygen
    WORKING_DIRECTORY
        ${magicxx_SOURCE_DIR}
    COMMENT
        "Installing the dependencies..."
    VERBATIM
)

add_custom_target(initialize_and_update_git_submodules
    COMMAND
        git submodule update --init --recursive
    WORKING_DIRECTORY
        ${magicxx_SOURCE_DIR}
    COMMENT
        "Initializing and updating git submodules..."
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
