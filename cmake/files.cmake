# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# External dependency files
# -----------------------------------------------------------------------------
set(magic_DEFAULT_DATABASE_FILE
    ${magic_DEFAULT_DATABASE_DIR}/magic
)

set(magic_DEFAULT_COMPILED_DATABASE_FILE
    ${magic_DEFAULT_DATABASE_DIR}/magic.mgc
)

set(magic_LICENSE_FILE
    ${magic_DIR}/COPYING
)

set(gnurx_LICENSE_FILE
    ${gnurx_DIR}/COPYING.LIB
)

# -----------------------------------------------------------------------------
# Project files
# -----------------------------------------------------------------------------
set(magicxx_LICENSE_FILE
    ${magicxx_SOURCE_DIR}/COPYING.LESSER
)

set(magicxx_CHANGELOG_FILE
    ${magicxx_SOURCE_DIR}/CHANGELOG.md
)

set(magicxx_CPACK_PACKAGE_DESCRIPTION_FILE
    ${magicxx_SOURCE_DIR}/cmake/cpack_package_description_file.txt
)

set(magicxx_DEFAULT_DATABASE_FILE
    ${magicxx_DEFAULT_DATABASE_DIR}/magic
)

set(magicxx_DEFAULT_COMPILED_DATABASE_FILE
    ${magicxx_DEFAULT_DATABASE_DIR}/magic.mgc
)

set(magicxx_DEFAULT_DATABASE_FILES
    ${magicxx_DEFAULT_DATABASE_FILE}
    ${magicxx_DEFAULT_COMPILED_DATABASE_FILE}
)

set(magicxx_INSTALLED_DEFAULT_DATABASE_FILE
    ${magicxx_INSTALL_DEFAULT_DATABASES_DIR}/magic
)

set(magicxx_README_FILE
    ${magicxx_SOURCE_DIR}/README.md
)

set(magicxx_CODE_OF_CONDUCT_FILE
    ${magicxx_SOURCE_DIR}/CODE_OF_CONDUCT.md
)

set(magicxx_CONTRIBUTING_FILE
    ${magicxx_SOURCE_DIR}/CONTRIBUTING.md
)

set(magicxx_SECURITY_FILE
    ${magicxx_SOURCE_DIR}/SECURITY.md
)

set(magicxx_CXX_STYLE_GUIDE_FILE
    ${magicxx_SOURCE_DIR}/STYLE_GUIDE.md
)

set(magicxx_CMAKE_STYLE_GUIDE_FILE
    ${magicxx_CMAKE_DIR}/STYLE_GUIDE.md
)

set(magicxx_AGENTS_FILE
    ${magicxx_SOURCE_DIR}/AGENTS.md
)

set(magicxx_EXAMPLES_FILE
    ${magicxx_EXAMPLES_DIR}/magic_examples.cpp
)

# -----------------------------------------------------------------------------
# Source files for formatting
# -----------------------------------------------------------------------------
set(magicxx_SOURCE_FILES_TO_FORMAT
    ${magicxx_HEADER_FILES}
    ${magicxx_SOURCE_FILES}
    ${magicxx_examples_SOURCE_FILES}
    ${magicxx_tests_SOURCE_FILES}
)
