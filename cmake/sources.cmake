# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Windows-specific source files
# -----------------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
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

# -----------------------------------------------------------------------------
# Magic library source files
# -----------------------------------------------------------------------------
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

# -----------------------------------------------------------------------------
# Magicxx library source files
# -----------------------------------------------------------------------------
set(magicxx_HEADER_FILES
    ${magicxx_INCLUDE_DIR}/magic.hpp
    ${magicxx_INCLUDE_DIR}/magic_exception.hpp
    ${magicxx_INCLUDE_DIR}/percentage.hpp
    ${magicxx_INCLUDE_DIR}/progress_tracker.hpp
    ${magicxx_INCLUDE_DIR}/utility.hpp
)

set(magicxx_SOURCE_FILES
    ${magicxx_SOURCES_DIR}/magic.cpp
)

# -----------------------------------------------------------------------------
# Magicxx example source files
# -----------------------------------------------------------------------------
set(magicxx_examples_SOURCE_FILES
    ${magicxx_EXAMPLES_DIR}/magic_examples.cpp
)

# -----------------------------------------------------------------------------
# Magicxx test source files
# -----------------------------------------------------------------------------
set(magicxx_tests_SOURCE_FILES
    ${magicxx_TESTS_DIR}/magic_check_test.cpp
    ${magicxx_TESTS_DIR}/magic_compile_test.cpp
    ${magicxx_TESTS_DIR}/magic_flags_mask_test.cpp
    ${magicxx_TESTS_DIR}/magic_flags_test.cpp
    ${magicxx_TESTS_DIR}/magic_identify_container_test.cpp
    ${magicxx_TESTS_DIR}/magic_identify_directory_test.cpp
    ${magicxx_TESTS_DIR}/magic_identify_file_test.cpp
    ${magicxx_TESTS_DIR}/magic_load_database_file_test.cpp
    ${magicxx_TESTS_DIR}/magic_open_close_test.cpp
    ${magicxx_TESTS_DIR}/magic_parameters_test.cpp
    ${magicxx_TESTS_DIR}/magic_percentage_test.cpp
    ${magicxx_TESTS_DIR}/magic_progress_tracker_test.cpp
    ${magicxx_TESTS_DIR}/magic_special_members_test.cpp
    ${magicxx_TESTS_DIR}/magic_to_string_test.cpp
    ${magicxx_TESTS_DIR}/magic_version_test.cpp
    ${magicxx_TESTS_DIR}/main.cpp
)
