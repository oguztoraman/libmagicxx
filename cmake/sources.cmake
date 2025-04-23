# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(gnurx_SOURCE_FILES
        ${gnurx_DIR}/regex.c
    )

    set(magic_OS_REPLACEMENT_SOURCE_FILES
        ${magic_SOURCE_DIR}/asctime_r.c
        ${magic_SOURCE_DIR}/ctime_r.c
        ${magic_SOURCE_DIR}/getline.c
        ${magic_SOURCE_DIR}/gmtime_r.c
        ${magic_SOURCE_DIR}/localtime_r.c
        ${magic_SOURCE_DIR}/pread.c
        ${magic_SOURCE_DIR}/strcasestr.c
        ${magic_SOURCE_DIR}/strlcpy.c
    )
endif()

set(magic_HEADER_FILES
    ${magic_SOURCE_DIR}/magic.h
)

set(magic_SOURCE_FILES
    ${magic_SOURCE_DIR}/apprentice.c
    ${magic_SOURCE_DIR}/apptype.c
    ${magic_SOURCE_DIR}/ascmagic.c
    ${magic_SOURCE_DIR}/buffer.c
    ${magic_SOURCE_DIR}/cdf.c
    ${magic_SOURCE_DIR}/cdf_time.c
    ${magic_SOURCE_DIR}/compress.c
    ${magic_SOURCE_DIR}/der.c
    ${magic_SOURCE_DIR}/encoding.c
    ${magic_SOURCE_DIR}/fmtcheck.c
    ${magic_SOURCE_DIR}/fsmagic.c
    ${magic_SOURCE_DIR}/funcs.c
    ${magic_SOURCE_DIR}/is_csv.c
    ${magic_SOURCE_DIR}/is_json.c
    ${magic_SOURCE_DIR}/is_simh.c
    ${magic_SOURCE_DIR}/is_tar.c
    ${magic_SOURCE_DIR}/magic.c
    ${magic_SOURCE_DIR}/print.c
    ${magic_SOURCE_DIR}/readcdf.c
    ${magic_SOURCE_DIR}/readelf.c
    ${magic_SOURCE_DIR}/softmagic.c
    ${magic_OS_REPLACEMENT_SOURCE_FILES}
)

set(magicxx_HEADER_FILES
    ${magicxx_INCLUDE_DIR}/file_concepts.hpp
    ${magicxx_INCLUDE_DIR}/magic.hpp
    ${magicxx_INCLUDE_DIR}/magic_exception.hpp
    ${magicxx_INCLUDE_DIR}/utility.hpp
)

set(magicxx_SOURCE_FILES
    ${magicxx_SOURCES_DIR}/magic.cpp
)
