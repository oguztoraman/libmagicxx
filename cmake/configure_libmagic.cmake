# SPDX-FileCopyrightText: Copyright (c) 2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

message(STATUS "Initializing and updating git submodules...")
execute_process(
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${magicxx_SOURCE_DIR}
    RESULT_VARIABLE submodule_result
)
if (submodule_result)
    message(FATAL_ERROR "Failed to update git submodules.")
endif()

message(STATUS "Configuring the file...")
execute_process(
    COMMAND autoreconf -f -i
    WORKING_DIRECTORY ${magic_DIR}
    RESULT_VARIABLE autoreconf_result
)
if (autoreconf_result)
    message(FATAL_ERROR "Failed to run autoreconf.")
endif()

execute_process(
    COMMAND ./configure --enable-static=yes --enable-shared=yes
        --disable-zlib --disable-bzlib
        --disable-xzlib --disable-zstdlib
        --disable-lzlib --silent
    WORKING_DIRECTORY ${magic_DIR}
    RESULT_VARIABLE configure_result
)
if (configure_result)
    message(FATAL_ERROR "Failed to configure file.")
endif()

execute_process(
    COMMAND make magic.h
    WORKING_DIRECTORY ${magic_SOURCE_DIR}
    RESULT_VARIABLE make_result
)
if (make_result)
    message(FATAL_ERROR "Failed to make.")
endif()