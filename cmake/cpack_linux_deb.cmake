# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Debian package configuration
# -----------------------------------------------------------------------------
set(CPACK_DEB_COMPONENT_INSTALL
    OFF
)

set(CPACK_DEBIAN_PACKAGE_EPOCH
    1
)

set(CPACK_DEBIAN_PACKAGE_RELEASE
    1
)

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    ${CMAKE_SYSTEM_PROCESSOR}
)

set(CPACK_DEBIAN_COMPRESSION_TYPE
    zstd
)
