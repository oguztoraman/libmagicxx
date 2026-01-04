# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# General CPack settings
# -----------------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CPACK_STRIP_FILES ON)
endif()

set(CPACK_THREADS
    0
)

set(CPACK_PACKAGE_CHECKSUM
    SHA256
)

# -----------------------------------------------------------------------------
# Package metadata
# -----------------------------------------------------------------------------
set(CPACK_PACKAGE_NAME
    libmagicxx
)

set(CPACK_PACKAGE_VENDOR
    "Oğuz Toraman <oguz.toraman@tutanota.com>"
)

set(CPACK_PACKAGE_CONTACT
    ${CPACK_PACKAGE_VENDOR}
)

set(CPACK_PACKAGE_VERSION
    ${magicxx_VERSION}
)

set(CPACK_PACKAGE_DESCRIPTION_FILE
    ${magicxx_CPACK_PACKAGE_DESCRIPTION_FILE}
)

set(CPACK_PACKAGE_DIRECTORY
    ${magicxx_PACKAGES_DIR}
)

set(CPACK_RESOURCE_FILE_LICENSE
    ${magicxx_LICENSE_FILE}
)

set(CPACK_MONOLITHIC_INSTALL
    ON
)

include(cmake/cpack_source.cmake)

# -----------------------------------------------------------------------------
# Platform-specific generators
# -----------------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR "DEB;RPM;ZIP")
    include(cmake/cpack_linux_deb.cmake)
    include(cmake/cpack_linux_rpm.cmake)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_GENERATOR "NSIS64;ZIP")
    include(cmake/cpack_windows.cmake)
endif()

include(CPack)
