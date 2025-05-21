# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

set(magic_DIR
    ${magicxx_SOURCE_DIR}/external/file
)

set(magic_INCLUDE_DIR
    ${magic_DIR}
    ${magic_DIR}/src
)

set(magic_SOURCE_DIR
    ${magic_DIR}/src
)

set(magic_DEFAULT_DATABASE_DIR
    ${magic_DIR}/magic
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(gnurx_DIR
        ${magicxx_SOURCE_DIR}/external/libgnurx
    )

    set(gnurx_INCLUDE_DIR
        ${gnurx_DIR}
    )
endif()

set(magicxx_DEFAULT_DATABASE_DIR
    ${magicxx_SOURCE_DIR}/databases
)

set(magicxx_PACKAGES_DIR
    ${magicxx_SOURCE_DIR}/packages
)

set(magicxx_INCLUDE_DIR
    ${magicxx_SOURCE_DIR}/include/magicxx
)

set(magicxx_SOURCES_DIR
    ${magicxx_SOURCE_DIR}/sources
)

include(GNUInstallDirs)

set(magicxx_INSTALL_INCLUDEDIR
    ${CMAKE_INSTALL_INCLUDEDIR}/magicxx
)

set(magicxx_INSTALL_LIBDIR
    ${CMAKE_INSTALL_LIBDIR}/magicxx
)

set(magicxx_INSTALL_DOCDIR
    ${CMAKE_INSTALL_DOCDIR}
)

set(magicxx_CMAKE_INSTALL_LIBDIR
    ${CMAKE_INSTALL_LIBDIR}/cmake/magicxx
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(magicxx_DEFAULT_DATABASES_INSTALL_DIR
        "C:\\Program Files\\magicxx\\databases"
    )
else()
    set(magicxx_DEFAULT_DATABASES_INSTALL_DIR
        "/usr/share/magicxx/databases"
    )
endif()
