# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Doxygen documentation configuration
# Generates API documentation for libmagicxx using Doxygen.
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.31.6)

project(magicxx_documentation
    DESCRIPTION "Doxygen documentation for Libmagicxx."
)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

find_package(Doxygen REQUIRED dot)

# -----------------------------------------------------------------------------
# Doxygen settings
# -----------------------------------------------------------------------------

set(DOXYGEN_PROJECT_NAME
    Libmagicxx
)

set(DOXYGEN_PROJECT_NUMBER
    "v${magicxx_VERSION}"
)

set(DOXYGEN_PROJECT_BRIEF
    ${magicxx_DESCRIPTION}
)

set(DOXYGEN_FULL_PATH_NAMES
    NO
)

set(DOXYGEN_TIMESTAMP
    DATETIME
)

set(DOXYGEN_EXTRACT_ALL
    YES
)

set(DOXYGEN_EXTRACT_PRIVATE
    YES
)

set(DOXYGEN_EXTRACT_PRIV_VIRTUAL
    YES
)

set(DOXYGEN_EXTRACT_PACKAGE
    YES
)

set(DOXYGEN_EXTRACT_STATIC
    YES
)

set(DOXYGEN_EXTRACT_ANON_NSPACES
    YES
)

set(DOXYGEN_USE_MDFILE_AS_MAINPAGE
    ${magicxx_README_FILE}
)

set(DOXYGEN_HTML_COLORSTYLE
    TOGGLE
)

set(DOXYGEN_DISABLE_INDEX
    NO
)

set(DOXYGEN_GENERATE_TREEVIEW
    YES
)

set(DOXYGEN_FULL_SIDEBAR
    NO
)

set(DOXYGEN_SHOW_ENUM_VALUES
    YES
)

set(DOXYGEN_GENERATE_LATEX
    NO
)

set(DOXYGEN_OUTPUT_DIRECTORY
    ${magicxx_DOCUMENTATION_DIR}
)

set(DOXYGEN_EXAMPLE_PATH
    ${magicxx_EXAMPLES_DIR}
)

set(DOXYGEN_MARKDOWN_ID_STYLE
    GITHUB
)

set(DOXYGEN_EXTENSION_MAPPING
    LESSER=md
)

# -----------------------------------------------------------------------------
# Documentation targets
# -----------------------------------------------------------------------------

add_custom_target(delete_old_docs
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${magicxx_DOCUMENTATION_SITE_DIR}
    COMMENT
        "Deleting old documentation..."
    VERBATIM
)

#-----------------------------------------------------------------------------
# Doxygen documentation generation
#-----------------------------------------------------------------------------

doxygen_add_docs(
    magicxx_documentation
        ${magic_HEADER_FILES}
        ${magicxx_HEADER_FILES}
        ${magicxx_SOURCE_FILES}
        ${magicxx_DOCUMENTATION_FILES}
    WORKING_DIRECTORY
        ${magicxx_SOURCE_DIR}
    COMMENT
        "Generating documentation..."
)

add_dependencies(magicxx_documentation delete_old_docs)

# -----------------------------------------------------------------------------
# Copy 404.html for GitHub Pages redirect support
# When users click .md links on the docs site, the 404 page redirects them
# to the correct Doxygen-generated HTML page.
# -----------------------------------------------------------------------------

add_custom_command(TARGET magicxx_documentation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${magicxx_DOCUMENTATION_DIR}/404.html
        ${magicxx_DOCUMENTATION_SITE_DIR}/404.html
    COMMENT
        "Copying 404.html for redirect support..."
    VERBATIM
)
