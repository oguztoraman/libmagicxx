# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

install(
    FILES
        ${magicxx_LICENSE_FILE}
    DESTINATION
        ${magicxx_INSTALL_LICENSEDIR}
    COMPONENT
        License
)

install(
    FILES
        ${magic_LICENSE_FILE}
    DESTINATION
        ${magic_INSTALL_LICENSEDIR}
    COMPONENT
        License
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    install(
        FILES
            ${gnurx_LICENSE_FILE}
        DESTINATION
            ${gnurx_INSTALL_LICENSEDIR}
        COMPONENT
            License
    )
endif()

install(
    FILES
        ${magicxx_DEFAULT_DATABASE_FILES}
    DESTINATION
        ${magicxx_DEFAULT_DATABASES_INSTALL_DIR}
    COMPONENT
        Data
)

install(
    FILES
        ${magicxx_HEADER_FILES}
    DESTINATION
        ${magicxx_INSTALL_INCLUDEDIR}
    COMPONENT
        Development
)

install(
    EXPORT
        magicxxTargets
    NAMESPACE
        recognition::
    DESTINATION
        ${magicxx_CMAKE_INSTALL_LIBDIR}
    COMPONENT
        Development
)

install(
    FILES
        ${magicxx_CMAKE_PACKAGE_CONFIG_FILE}
        ${magicxx_CMAKE_PACKAGE_CONFIG_VERSION_FILE}
    DESTINATION
        ${magicxx_CMAKE_INSTALL_LIBDIR}
    COMPONENT
        Development
)

install(
    TARGETS
        magicxx
    EXPORT
        magicxxTargets
    FILE_SET
        magicxxHeaders
    LIBRARY DESTINATION
        ${magicxx_INSTALL_LIBDIR}
    COMPONENT
        Library
)

install(
    TARGETS
        magicxx_static
    EXPORT
        magicxxTargets
    FILE_SET
        magicxx_staticHeaders
    ARCHIVE DESTINATION
        ${magicxx_INSTALL_LIBDIR}
    COMPONENT
        Archive
)

install(
    DIRECTORY
        documentation/html
    DESTINATION
        ${magicxx_INSTALL_DOCDIR}
    COMPONENT
        Documentation
)
