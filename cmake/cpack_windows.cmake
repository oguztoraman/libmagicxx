# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# NSIS installer configuration (Windows)
# -----------------------------------------------------------------------------
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL
    ON
)

set(CPACK_NSIS_MODIFY_PATH
    ON
)

set(CPACK_NSIS_DISPLAY_NAME
    ${CPACK_PACKAGE_NAME}
)

set(CPACK_NSIS_PACKAGE_NAME
    ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}
)

set(CPACK_NSIS_CONTACT
    ${CPACK_PACKAGE_CONTACT}
)

set(CPACK_NSIS_MENU_LINKS
    "https://oguztoraman.github.io/libmagicxx/" "Libmagicxx Web Site"
)
