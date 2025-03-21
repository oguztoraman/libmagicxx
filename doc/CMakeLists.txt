# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.30.8)

project(magicxx_documentation)

find_package(Doxygen REQUIRED dot)

set(DOXYGEN_PROJECT_NAME           Libmagicxx)
set(DOXYGEN_PROJECT_NUMBER         "v${magicxx_VERSION}")
set(DOXYGEN_PROJECT_BRIEF          ${magicxx_DESCRIPTION})
set(DOXYGEN_FULL_PATH_NAMES        NO)
set(DOXYGEN_TIMESTAMP              DATETIME)
set(DOXYGEN_EXTRACT_ALL            YES)
set(DOXYGEN_EXTRACT_PRIVATE        YES)
set(DOXYGEN_EXTRACT_PRIV_VIRTUAL   YES)
set(DOXYGEN_EXTRACT_PACKAGE        YES)
set(DOXYGEN_EXTRACT_STATIC         YES)
set(DOXYGEN_EXTRACT_ANON_NSPACES   YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${magicxx_SOURCE_DIR}/README.md)
set(DOXYGEN_HTML_COLORSTYLE        TOGGLE)
set(DOXYGEN_DISABLE_INDEX          YES)
set(DOXYGEN_GENERATE_TREEVIEW      YES)
set(DOXYGEN_SHOW_ENUM_VALUES       YES)
set(DOXYGEN_GENERATE_LATEX         NO)
set(DOXYGEN_GENERATE_MAN           YES)
set(DOXYGEN_OUTPUT_DIRECTORY       ${magicxx_SOURCE_DIR}/doc)

doxygen_add_docs(
    magicxx_documentation
        inc
        src
        file/src/magic.h
        CHANGELOG.md
        CODE_OF_CONDUCT.md
        CONTRIBUTING.md
        README.md
        SECURITY.md
    WORKING_DIRECTORY
        ${magicxx_SOURCE_DIR}
    COMMENT
        "Building documentation..."
)
