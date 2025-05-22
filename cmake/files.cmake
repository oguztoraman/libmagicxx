# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

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
    ${magicxx_DEFAULT_DATABASES_INSTALL_DIR}/magic
)

set(magicxx_SOURCE_FILES_TO_FORMAT
    ${magicxx_HEADER_FILES}
    ${magicxx_SOURCE_FILES}
    ${magicxx_examples_SOURCE_FILES}
    ${magicxx_tests_SOURCE_FILES}
)
