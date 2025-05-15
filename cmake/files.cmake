# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

set(magicxx_LICENSE_FILE
    ${magicxx_SOURCE_DIR}/COPYING.LESSER
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
